package Lab3

import chisel3._
import chisel3.util._

case class MatrixMultiplicationTopConfigs(
    /* Fixed */
    matDWidth: Int = 8,
    resDWidth: Int = 32,
    /* Configurable */
    matSize: Int = 16,
    memSize: Int = 0x1000,
    addrWidth: Int = 32,
    baseAddressA: Int = 0x0000,
    baseAddressB: Int = 0x0200,
    baseAddressC: Int = 0x0400,
    isSignedA: Boolean = false,
    isSignedB: Boolean = false,
    /* Enum */
    accessMemOp: Int = 0,
    computeOp: Int = 1
)

class MMTopModule(cfg: MatrixMultiplicationTopConfigs) extends Module {

  /* Define I/O */
  val io = IO(new Bundle {
    val operation = Input(UInt(1.W))
    val dataIn = Input(SInt((cfg.matDWidth + 1).W))
    val address = Input(UInt(cfg.addrWidth.W))
    val enable = Input(Bool())
    val writeEnable = Input(Bool())
    val start = Input(Bool())

    val dataOut = Output(SInt(cfg.resDWidth.W))
    val busy = Output(Bool())
  })

  assert(cfg.matDWidth == 8, "Only support 8-bit data input width")
  assert(cfg.resDWidth == 32, "Only support 32-bit data output width")

  /* Define submodule */
  val matrixMultiplicationModule = Module(
    new MatrixMultiplicationModule(
      MatrixMultiplicationConfigs(
        matSize = cfg.matSize,
        memSize = cfg.memSize,
        addrWidth = cfg.addrWidth,
        baseAddressA = cfg.baseAddressA,
        baseAddressB = cfg.baseAddressB,
        baseAddressC = cfg.baseAddressC,
        isSignedA = cfg.isSignedA,
        isSignedB = cfg.isSignedB
      )
    )
  )

  /* Default input & output */
  matrixMultiplicationModule.io.start := (io.operation === cfg.computeOp.U) & io.start
  matrixMultiplicationModule.io.dataIn := io.dataIn.asUInt
  matrixMultiplicationModule.io.address := io.address
  matrixMultiplicationModule.io.writeEnable := (io.operation === cfg.accessMemOp.U) & io.enable & io.writeEnable
  io.dataOut := matrixMultiplicationModule.io.dataOut.asSInt
  io.busy := !matrixMultiplicationModule.io.finish
}
