package ROCC_Lab

import chisel3._
import chisel3.util._
import freechips.rocketchip.tile._
import org.chipsalliance.cde.config._
import freechips.rocketchip.diplomacy._
import freechips.rocketchip.rocket._

import ROCC_Lab_config._

class DMA_read_req(cfg: MatrixMultiplicationConfigs) extends Bundle {
  val len = Input(UInt(cfg.addrWidth.W)) // 单位为一个64b
  val baseAddr_DRAM = Input(UInt(32.W))
  val baseAddr_OnChipMem = Input(UInt(cfg.addrWidth.W))
}

class DMA_reader_Ctl(cfg: MatrixMultiplicationConfigs)(implicit p: Parameters)
    extends LazyModule {
  val nXacts = 4 // DMA 通道数
  val outFlits = 32 // DMA 内部 buffer 大小
  val maxBytes = 64 // DMA 每个 TileLink 请求最大字节数
  val dmaReader = LazyModule(new StreamReader(nXacts, outFlits, maxBytes)(p))
  val dmaReader_node = dmaReader.node

  lazy val module = new DMA_reader_Ctl_Impl
  class DMA_reader_Ctl_Impl extends LazyModuleImp(this) {
    val io = IO(new Bundle {
      val req = Flipped(Decoupled(new DMA_read_req(cfg)))
      val busy = Output(Bool())

      // Access on chip mem (Read)
      val accessOnChipMem = Decoupled(new ReqPackage(cfg))
    })

    val arbiter = Module(new RRArbiter(new DMA_read_req(cfg), 1))
    arbiter.io.in(0) <> io.req

    val dataBits = dmaReader.core.module.dataBits

    val len_64b = RegInit(0.U(16.W))
    val cur_64b_len = RegInit(0.U(10.W))
    val cur_block_len = RegInit(0.U(13.W))
    val baseAddr_DRAM = RegInit(0.U(32.W))
    val baseAddr_OnChipMem = RegInit(0.U(cfg.addrWidth.W))
    val total_nbytes = len_64b << log2Ceil(dataBits / 8)
    val p_page_id = RegInit(0.U(32.W))
    val v_addr_ptr = RegInit(0.U(32.W))
    val write_array_addr_ptr = RegInit(0.U(16.W))
    val read_ptr = RegInit(0.U(12.W))
    val final_v_addr =
      baseAddr_DRAM + total_nbytes // the final final_v_addr will not be accessed
    val dma_read_type = RegInit(0.U(2.W)) // from p2s or normal access

    val main_idle :: pre_process :: query_TLB :: query_wait_resp :: dma_request :: dma_reading :: dma_finish :: Nil =
      Enum(7)
    val main_state = RegInit(main_idle)
    val busy = (main_state =/= main_idle)
    io.busy := busy
    arbiter.io.out.ready := false.B

    // DMA 读ram
    val dmaReaderIO = dmaReader.module.io // icenet DMA 提供的读内存接口
    val canRead = busy
    val canSendReq = RegInit(false.B)
    dmaReaderIO.req.valid := false.B
    dmaReaderIO.out.ready := false.B
    // init
    dmaReaderIO.req.bits := DontCare
    dmaReaderIO.resp.ready := dmaReaderIO.resp.valid

    // 读出来的数据放入queue
    val data_queue = Module(new Queue(UInt(64.W), 6))
    data_queue.io.enq.valid := false.B
    data_queue.io.enq.bits := 0.U
    // queue state
    val is_queue_empty = !data_queue.io.deq.valid
    val is_queue_has_val = data_queue.io.deq.valid

    // 运行主干状态
    val next_page_begin_addr = ((v_addr_ptr >> 12) + 1.U) << 12
    switch(main_state)
    {
      is(main_idle)
      {
        arbiter.io.out.ready := true.B
        write_array_addr_ptr := 0.U
        when(arbiter.io.out.fire)
        {
          val req_data = arbiter.io.out.bits
          len_64b := req_data.len
          baseAddr_DRAM := req_data.baseAddr_DRAM
          baseAddr_OnChipMem := req_data.baseAddr_OnChipMem
          v_addr_ptr := req_data.baseAddr_DRAM

          main_state := pre_process
        }
      }
      is(pre_process)
      {
        when(v_addr_ptr < final_v_addr)
        {
          main_state := query_TLB
          cur_block_len := Mux(
            final_v_addr > next_page_begin_addr,
            next_page_begin_addr - v_addr_ptr,
            final_v_addr - v_addr_ptr
          )
        }.otherwise
        {
          main_state := main_idle // 结束
        }
      }
      is(query_TLB)
      {
        p_page_id := v_addr_ptr >> 12
        canSendReq := true.B
        main_state := dma_request
      }
      is(dma_request)
      {
        val p_baseAddr_to_dma = (v_addr_ptr & "hFFF".U) + (p_page_id << 12)
        dmaReaderIO.req.valid := true.B
        when(dmaReaderIO.req.fire)
        {
          canSendReq := false.B
          dmaReaderIO.req.bits.address := p_baseAddr_to_dma
          dmaReaderIO.req.bits.length := cur_block_len
          dmaReaderIO.req.bits.partial := false.B
          main_state := dma_reading
        }
      }
      is(dma_reading)
      {
        val cur_64b_len = cur_block_len >> 3

        data_queue.io.enq.bits := dmaReaderIO.out.bits.data
        dmaReaderIO.out.ready := (read_ptr < cur_64b_len) && data_queue.io.enq.ready
        // fire的同时一定会有data_queue.io.enq.ready为真
        when(dmaReaderIO.out.fire)
        {
          read_ptr := read_ptr + 1.U
          data_queue.io.enq.valid := true.B
        }

        // dmaReaderIO.resp.ready := dmaReaderIO.resp.valid
        when(read_ptr === cur_64b_len) { main_state := dma_finish }
      }
      is(dma_finish)
      {
        // 先等queue中数据被取光
        when(is_queue_empty)
        {
          v_addr_ptr := next_page_begin_addr
          main_state := pre_process
          read_ptr := 0.U
        }
      }

    }

    // 从queue中取出数据到array或者p2s
    val deq_idle :: write_p2s :: write_array :: Nil = Enum(3)
    val deq_queue_state = RegInit(deq_idle)
    data_queue.io.deq.ready := false.B
    io.accessOnChipMem.valid := false.B
    io.accessOnChipMem.bits.optype := cfg.onChipWrite
    io.accessOnChipMem.bits.dataWrittenToOnChipMem := 0.S
    io.accessOnChipMem.bits.addr := 0.U
    switch(deq_queue_state)
    {
      is(deq_idle)
      {
        // 队里有东西
        when(data_queue.io.deq.valid)
        {
          deq_queue_state := write_array
        }
      }
      is(write_array)
      {
        io.accessOnChipMem.valid := is_queue_has_val

        when(io.accessOnChipMem.fire)
        {
          // deq
          data_queue.io.deq.ready := true.B
          // queue head val
          val data = data_queue.io.deq.bits
          write_array_addr_ptr := write_array_addr_ptr + 1.U
          io.accessOnChipMem.bits.addr := baseAddr_OnChipMem + write_array_addr_ptr
          io.accessOnChipMem.bits.dataWrittenToOnChipMem := data.asSInt
        }

        when(is_queue_empty)
        {
          deq_queue_state := deq_idle
        }
      }
    }
  }
}
