package ROCC_Lab

import chisel3._
import chisel3.util._
import freechips.rocketchip.tile._
import freechips.rocketchip.subsystem._
import org.chipsalliance.cde.config._
import freechips.rocketchip.diplomacy._
import freechips.rocketchip.util._
import freechips.rocketchip.tilelink._

import ROCC_Lab_config._
import GEMM._

class RoccInterface(cfg: MatrixMultiplicationConfigs, opcode: OpcodeSet)(
    implicit p: Parameters
) extends LazyRoCC(opcode, nPTWPorts = 1) {
  val dma_reader = LazyModule(new DMA_reader_Ctl(cfg))
  val dma_writer = LazyModule(new DMA_writer_Ctl(cfg))
  val dmaReaderNode = dma_reader.dmaReader_node
  val dmaWriterNode = dma_writer.dmaWriter_node
  tldmaNode :=* dmaReaderNode
  tldmaNode :=* dmaWriterNode

  lazy val module = new RoccInterfaceImpl
  class RoccInterfaceImpl extends LazyRoCCModuleImp(this) {
    val dmaReader_module = dma_reader.module
    val dmaWriter_module = dma_writer.module
    val gemm = Module(new MatrixMultiplicationModule(cfg))
    val arbiter = Module(new AccessOnChipMem_Arb(cfg))

    val s_idle :: s_load :: s_ld_rev :: s_store :: s_compute :: s_wait :: Nil =
      Enum(6)
    val state = RegInit(s_idle)
    io.busy := !(state === s_idle)
    io.cmd.ready := (state === s_idle)

    val rs1_reg = RegInit(0.U(64.W))
    val rs2_reg = RegInit(0.U(32.W))

    val gemm_matARows = RegInit(0.U(cfg.indexWidth.W))
    val gemm_matACols = RegInit(0.U(cfg.indexWidth.W))
    val gemm_matBRows = RegInit(0.U(cfg.indexWidth.W))
    val gemm_matBCols = RegInit(0.U(cfg.indexWidth.W))

    when(io.cmd.fire && io.cmd.bits.inst.funct === cfg.infoISA) {
      gemm_matARows := io.cmd.bits.rs1(63, 32)
      gemm_matACols := io.cmd.bits.rs1(31, 0)
      gemm_matBRows := io.cmd.bits.rs1(31, 0)
      gemm_matBCols := io.cmd.bits.rs2(31, 0)
    }

    dmaReader_module.io.req.valid := (state === s_load)
    dmaWriter_module.io.req.valid := (state === s_store)
    DMA_read_conn(req = dmaReader_module.io.req, rs1 = rs1_reg, rs2 = rs2_reg)
    DMA_write_conn(req = dmaWriter_module.io.req, rs1 = rs1_reg, rs2 = rs2_reg)

    gemm.io.start := false.B

    gemm.io.dataIn := arbiter.io.dataOut.asUInt
    gemm.io.writeEnable := arbiter.io.enable && arbiter.io.writeEn
    gemm.io.address := arbiter.io.addr
    arbiter.io.dataIn := gemm.io.dataOut
    when(arbiter.io.addr < cfg.baseAddressB.U) {
      gemm.io.select := cfg.selectA.U
    }.elsewhen(arbiter.io.addr < cfg.baseAddressC.U) {
      gemm.io.select := cfg.selectB.U
    }.otherwise {
      gemm.io.select := cfg.selectC.U
    }

    gemm.io.matARows := gemm_matARows
    gemm.io.matACols := gemm_matACols
    gemm.io.matBRows := gemm_matBRows
    gemm.io.matBCols := gemm_matBCols

    arbiter.io.request(0) <> dmaReader_module.io.accessOnChipMem
    arbiter.io.request(1) <> dmaWriter_module.io.accessOnChipMem
    dmaWriter_module.io.dataReadFromOnChipMem := arbiter.io.dataReadFromOnChipMem

    switch(state)
    {
      is(s_idle)
      {
        when(io.cmd.fire)
        {
          rs1_reg := io.cmd.bits.rs1
          rs2_reg := io.cmd.bits.rs2
          when(io.cmd.bits.inst.funct === cfg.loadISA)
          { state := s_load }
            .elsewhen(io.cmd.bits.inst.funct === cfg.storeISA)
          { state := s_store }
            .elsewhen(io.cmd.bits.inst.funct === cfg.computeISA)
          { state := s_compute }
        }
      }
      is(s_load)
      {
        when(dmaReader_module.io.req.fire)
        { state := s_wait }
      }
      is(s_compute)
      {
        gemm.io.start := true.B
        state := s_wait
      }
      is(s_store)
      {
        when(dmaWriter_module.io.req.fire)
        { state := s_wait }
      }
      is(s_wait)
      {
        val busy =
          (dmaReader_module.io.busy) || (dmaWriter_module.io.busy) || (gemm.io.busy)
        state := Mux(busy, state, s_idle)
      }
    }
  }

  def DMA_read_conn(
      req: DecoupledIO[DMA_read_req],
      rs1: UInt,
      rs2: UInt
  ): Unit = {
    req.bits.len := rs1(63, 32)
    req.bits.baseAddr_OnChipMem := rs1(31, 0)
    req.bits.baseAddr_DRAM := rs2
  }

  def DMA_write_conn(
      req: DecoupledIO[DMA_write_req],
      rs1: UInt,
      rs2: UInt
  ): Unit = {
    req.bits.len := rs1(63, 32)
    req.bits.baseAddr_OnChipMem := rs1(31, 0)
    req.bits.baseAddr_DRAM := rs2
  }

}

class WithRoccInterface
    extends Config((site, here, up) => { case BuildRoCC =>
      List((p: Parameters) => {
        val rocc =
          LazyModule(
            new RoccInterface(
              new MatrixMultiplicationConfigs,
              OpcodeSet.custom0
            )(p)
          )
        rocc
      })
    })
