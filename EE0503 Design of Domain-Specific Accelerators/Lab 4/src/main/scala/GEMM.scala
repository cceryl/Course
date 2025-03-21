package Lab4

import chisel3._
import chisel3.util._

/* This module is used only for given testbench */

case class MatMulConfig(
    /* Fixed */
    vecDWidth: Int = 8,
    resDWidth: Int = 32,
    /* Configurable */
    matSize: Int = 16,
    gemm_matsize: Int = 32,
    /* Enum */
    SEL_A: UInt = 0.U,
    SEL_B: UInt = 1.U,
    SEL_C: UInt = 2.U,
    OP_ACCESS_MEM: UInt = 1.U,
    OP_COMPUTE: UInt = 0.U
)

class GEMM_TOP(cfg: MatMulConfig) extends Module {

  val io = IO(new Bundle {
    val dataIn = Input(SInt((cfg.vecDWidth + 1).W))
    val addr = Input(UInt(cfg.resDWidth.W))
    val writeEn = Input(Bool())
    val enable = Input(Bool())
    val op = Input(UInt(2.W))
    val start = Input(Bool())

    val busy = Output(Bool())
    val dataOut = Output(SInt(cfg.resDWidth.W))
  })

  /* Matrix multiplication module */
  val elemCount = cfg.gemm_matsize * cfg.gemm_matsize
  val multiplicationModule = Module(
    new MatrixMultiplicationModule(
      MatrixMultiplicationConfigs(
        matDWidth = cfg.vecDWidth,
        resDWidth = cfg.resDWidth,
        matARows = cfg.gemm_matsize,
        matACols = cfg.gemm_matsize,
        matBRows = cfg.gemm_matsize,
        matBCols = cfg.gemm_matsize,
        calcCoreSize = cfg.matSize,
        memSize = 0x2000,
        addrWidth = 32,
        baseAddressA = 0x0000,
        baseAddressB = 0x0800,
        baseAddressC = 0x1000,
        isSignedA = false,
        isSignedB = true,
        debugEnable = false
      )
    )
  )

  /* Connection */
  multiplicationModule.io.start := io.start
  multiplicationModule.io.dataIn := io.dataIn.asUInt
  multiplicationModule.io.writeEnable := io.writeEn && io.enable && (io.op === cfg.OP_ACCESS_MEM)
  when(io.op === cfg.OP_ACCESS_MEM) {
    when(io.addr < (cfg.gemm_matsize * cfg.gemm_matsize).U) {
      multiplicationModule.io.select := 0.U
      multiplicationModule.io.address := io.addr
    }.elsewhen(io.addr < (2 * cfg.gemm_matsize * cfg.gemm_matsize).U) {
      multiplicationModule.io.select := 1.U
      multiplicationModule.io.address := io.addr - (cfg.gemm_matsize * cfg.gemm_matsize).U + 0x0800.U
    }.otherwise {
      multiplicationModule.io.select := 2.U

      multiplicationModule.io.address := (io.addr - (2 * elemCount).U) * 4.U + 0x1000.U
    }
  } otherwise {
    multiplicationModule.io.select := 3.U
    multiplicationModule.io.address := 0.U
  }

  io.busy := ~multiplicationModule.io.finish
  io.dataOut := multiplicationModule.io.dataOut
}
