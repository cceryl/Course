import chisel3._
import Lab6_FPGA._
import Lab6._

object generateVerilog extends App {
  val size = 4

  val config = new MatrixMultiplicationTopConfigs(
    matSize = size,
    memSize = 0x0250,
    addrWidth = 16,
    baseAddressA = 0x0000,
    baseAddressB = size * size,
    baseAddressC = size * size * 2,
    isSignedA = false,
    isSignedB = true
  )
  emitVerilog(
    new FPGAShellWrapper(config),
    Array("--target-dir", "sv/FPGAShell")
  )
}
