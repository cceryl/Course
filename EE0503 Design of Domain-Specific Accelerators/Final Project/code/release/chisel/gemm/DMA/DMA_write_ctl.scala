package ROCC_Lab

import chisel3._
import chisel3.util._
import freechips.rocketchip.tile._
import org.chipsalliance.cde.config._
import freechips.rocketchip.diplomacy._
import freechips.rocketchip.rocket._

import ROCC_Lab_config._

class DMA_write_req(cfg: MatrixMultiplicationConfigs) extends Bundle {
  val len = Input(UInt(cfg.addrWidth.W)) // 单位为一个64b
  val baseAddr_DRAM = Input(UInt(32.W))
  val baseAddr_OnChipMem = Input(UInt(cfg.addrWidth.W))
}

class DMA_writer_Ctl(cfg: MatrixMultiplicationConfigs)(implicit p: Parameters)
    extends LazyModule {
  val nXacts = 4 // DMA 通道数
  val outFlits = 32 // DMA 内部 buffer 大小
  val maxBytes = 64 // DMA 每个 TileLink 请求最大字节数
  val dmaWriter = LazyModule(new StreamWriter(nXacts, maxBytes)(p))
  val dmaWriter_node = dmaWriter.node

  lazy val module = new DMA_writer_Ctl_Impl
  class DMA_writer_Ctl_Impl extends LazyModuleImp(this) {

    val io = IO(new Bundle {
      val req = Flipped(Decoupled(new DMA_write_req(cfg)))
      val busy = Output(Bool())

      // Data access: array
      val accessOnChipMem = Decoupled(new ReqPackage(cfg))
      val dataReadFromOnChipMem = Input(SInt(64.W))
    })
    // init
    io.req.bits := DontCare
    io.req.ready := false.B
    io.accessOnChipMem.bits := DontCare

    val dataBits = dmaWriter.module.dataBits

    val len_64b = RegInit(0.U(16.W))
    val cur_block_len_byte = RegInit(0.U(16.W))
    val baseAddr_DRAM = RegInit(0.U(32.W))
    val baseAddr_OnChipMem = RegInit(0.U(cfg.addrWidth.W))
    val total_nbytes = len_64b << log2Ceil(dataBits / 8)
    val p_page_id = RegInit(0.U(32.W))
    val v_addr_ptr = RegInit(0.U(32.W))
    val array_ptr_for_accessOnChipMem = RegInit(0.U(16.W))
    val array_ptr_for_enq = RegInit(0.U(16.W))
    val cur_block_ptr_64b = RegInit(0.U(12.W))
    val final_v_addr =
      baseAddr_DRAM + total_nbytes // the final final_v_addr will not be accessed

    val main_idle :: pre_process :: query_TLB :: query_wait_resp :: dma_request :: dma_writing :: dma_finish :: Nil =
      Enum(7)
    val main_state = RegInit(main_idle)

    val busy = (main_state =/= main_idle)
    io.busy := busy

    val enq_idle :: read_array :: en_queue :: Nil = Enum(3)
    val enq_state = RegInit(enq_idle)

    // DMA 读ram
    val dmaWriterIO = dmaWriter.module.io // icenet DMA 提供的读内存接口
    val canRead = busy
    val canSendReq = RegInit(false.B)
    dmaWriterIO.req.valid := false.B
    dmaWriterIO.resp.ready := dmaWriterIO.resp.valid // 完成信号

    when(io.req.fire)
    {
      val req_data = io.req.bits
      len_64b := req_data.len
      baseAddr_DRAM := req_data.baseAddr_DRAM
      baseAddr_OnChipMem := req_data.baseAddr_OnChipMem
      v_addr_ptr := req_data.baseAddr_DRAM

      main_state := pre_process
      enq_state := read_array
    }

    // 读出来的数据放入queue
    val data_queue = Module(new Queue(UInt(64.W), 8))
    data_queue.io.enq.bits := 0.U
    data_queue.io.deq.ready := false.B
    data_queue.io.enq.valid := false.B

    val dataDeq_reg = RegInit(0.U(64.W))
    val is_queue_empty = !data_queue.io.deq.valid
    val is_queue_has_val = data_queue.io.deq.valid
    val is_queue_has_space = data_queue.io.enq.ready

    // 运行主干状态
    val next_page_begin_addr = ((v_addr_ptr >> 12) + 1.U) << 12
    val p_baseAddr_to_dma = (v_addr_ptr & "hFFF".U) + (p_page_id << 12)
    dmaWriterIO.resp.ready := dmaWriterIO.resp.valid
    dmaWriterIO.req.bits.address := p_baseAddr_to_dma
    dmaWriterIO.req.bits.length := cur_block_len_byte
    dmaWriterIO.req.bits.length := cur_block_len_byte
    dmaWriterIO.req.bits.length := cur_block_len_byte
    dmaWriterIO.in.valid := false.B
    dmaWriterIO.in.bits.keep := DontCare
    dmaWriterIO.in.bits.last := DontCare
    dmaWriterIO.in.bits.data := data_queue.io.deq.bits
    switch(main_state)
    {
      is(main_idle)
      {
        io.req.ready := true.B
        when(io.req.fire)
        {
          val req_data = io.req.bits
          len_64b := req_data.len
          baseAddr_DRAM := req_data.baseAddr_DRAM
          baseAddr_OnChipMem := req_data.baseAddr_OnChipMem
          v_addr_ptr := req_data.baseAddr_DRAM

          main_state := pre_process
          enq_state := read_array
        }
      }
      is(pre_process)
      {
        when(v_addr_ptr < final_v_addr)
        {
          main_state := query_TLB
          cur_block_len_byte := Mux(
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
        dmaWriterIO.req.valid := true.B
        when(dmaWriterIO.req.fire)
        {
          canSendReq := false.B
          // dmaWriterIO.req.bits.address := p_baseAddr_to_dma
          // dmaWriterIO.req.bits.length := cur_block_len_byte
          main_state := dma_writing
        }
      }
      is(dma_writing)
      {
        // 队列有数且dmaWriter可接收，就发送数据
        val cur_block_len_64b = (cur_block_len_byte >> 3)
        dmaWriterIO.in.valid := is_queue_has_val && (cur_block_ptr_64b < cur_block_len_64b)
        when(dmaWriterIO.in.fire) {
          // dmaWriterIO.in.bits.data:=data_queue.io.deq.bits
          cur_block_ptr_64b := cur_block_ptr_64b + 1.U
          data_queue.io.deq.ready := true.B
        }

        // 收到回应表示写完了，结束
        when(dmaWriterIO.resp.fire) {
          v_addr_ptr := next_page_begin_addr
          cur_block_ptr_64b := 0.U
          main_state := pre_process
        }
      }

    }

    io.accessOnChipMem.valid := false.B
    val data_arrive = RegNext(io.accessOnChipMem.fire)

    // array_ptr_for_enq 用来指示有没有读完所有数据
    // array_ptr_for_accessOnChipMem 用来指示读的地址，可能会发生回推的情况
    // array_ptr_for_accessOnChipMem:=array_ptr_for_accessOnChipMem+1.U 和回退不可能同时发生的
    // 此时必然是accessOnChipMem fire的下一个周期
    when(data_arrive)
    {
      // 可能是上一个延迟读写入后没空间了，需要回退array_ptr_for_accessOnChipMem
      when(!is_queue_has_space)
      {
        // roll back 上次读的重新读
        array_ptr_for_accessOnChipMem := array_ptr_for_accessOnChipMem - 1.U
      }.otherwise
      {
        data_queue.io.enq.valid := true.B
        data_queue.io.enq.bits := io.dataReadFromOnChipMem.asUInt
        array_ptr_for_enq := array_ptr_for_enq + 1.U
      }
    }

    switch(enq_state)
    {
      is(enq_idle)
      {}
      is(read_array)
      {
        // (array_ptr_for_accessOnChipMem<len_64b)是因为array_ptr_for_accessOnChipMem到达时，array_ptr_for_enq不一定到了，会导致多读
        val can_access_array =
          (array_ptr_for_enq < len_64b) && is_queue_has_space && (array_ptr_for_accessOnChipMem < len_64b)
        io.accessOnChipMem.valid := can_access_array

        when(io.accessOnChipMem.fire)
        {
          io.accessOnChipMem.bits.addr := baseAddr_OnChipMem + array_ptr_for_accessOnChipMem
          io.accessOnChipMem.bits.optype := cfg.onChipRead
          array_ptr_for_accessOnChipMem := array_ptr_for_accessOnChipMem + 1.U
        }

        // End 当且仅单array_ptr_for_enq===len_64b时，才表示所有数据已经入队
        when(array_ptr_for_enq === len_64b)
        {
          enq_state := enq_idle
          array_ptr_for_accessOnChipMem := 0.U
          array_ptr_for_enq := 0.U
        }
      }
    }
  }
}
