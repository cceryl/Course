package Lab3

import chisel3._
import chisel3.util._

case class MatMulConfig(
    vecDWidth: Int = 8, 
    resDWidth: Int = 32,
    matSize: Int = 8, 
    SEL_A: UInt = 0.U,
    SEL_B: UInt = 1.U,
    SEL_C: UInt = 2.U,
    OP_ACCESS_MEM: UInt = 1.U, 
    OP_COMPUTE: UInt = 0.U
) {
  val mat_elem_num = matSize * matSize
  val memory_size = mat_elem_num * 3
  val matA_baseAddr = 0
  val matB_baseAddr = mat_elem_num
  val matC_baseAddr = 2 * mat_elem_num
  val memory_width = resDWidth

  val LOAD = 0.U
  val COMPUTE = 1.U
  val STORE = 2.U

  val READ_mem = 0.U
  val WRITE_mem = 1.U
}

class MM_TOP(cfg: MatMulConfig) extends Module {

  /* Define I/O */
  val io = IO(new Bundle {
    val op = Input(UInt(1.W))
    val dataIn = Input(SInt((cfg.vecDWidth + 1).W))
    val addr = Input(UInt(cfg.memory_width.W))
    val enable = Input(Bool())
    val writeEn = Input(Bool())
    val start = Input(Bool())

    val dataOut = Output(SInt(cfg.resDWidth.W))
    val busy = Output(Bool())
  })

  assert(cfg.vecDWidth == 8, "Only support 8-bit data input width")
  assert(cfg.resDWidth == 32, "Only support 32-bit data output width")

  /* Define submodule */
  val matrixMultiplicationModule = Module(
    new MatrixMultiplicationModule(
      MatrixMultiplicationConfigs(
        matSize = cfg.matSize,
        memSize = cfg.mat_elem_num * (1 + 1 + 4),
        addrWidth = cfg.memory_width,
        baseAddressA = cfg.matA_baseAddr,
        baseAddressB = cfg.matB_baseAddr,
        baseAddressC = cfg.matC_baseAddr,
        isSignedA = false,
        isSignedB = true
      )
    )
  )

  /* Connect input & output */
  matrixMultiplicationModule.io.start := (io.op === cfg.OP_COMPUTE) & io.start
  matrixMultiplicationModule.io.dataIn := io.dataIn.asUInt
  when(io.addr < cfg.matC_baseAddr.U) {
    matrixMultiplicationModule.io.address := io.addr
  }.otherwise {
    matrixMultiplicationModule.io.address := (io.addr - cfg.matC_baseAddr.U) * 4.U + cfg.matC_baseAddr.U
  }
  matrixMultiplicationModule.io.writeEnable := (io.op === cfg.OP_ACCESS_MEM) & io.enable & io.writeEn
  io.dataOut := matrixMultiplicationModule.io.dataOut.asSInt
  io.busy := !matrixMultiplicationModule.io.finish
}
