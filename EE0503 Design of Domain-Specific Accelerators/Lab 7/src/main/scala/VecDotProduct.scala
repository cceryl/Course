package Lab3

import chisel3._
import chisel3.util._

case class VecDotProductConfigs(
    /* Configurable */
    vecDWidth: Int = 8,
    resDWidth: Int = 32,
    vecLen: Int = 16,
    isSignedA: Boolean = false,
    isSignedB: Boolean = false
)

class VecDotProductModule(cfg: VecDotProductConfigs) extends Module {

  val io = IO(new Bundle {
    val vecA = Input(Vec(cfg.vecLen, UInt(cfg.vecDWidth.W)))
    val vecB = Input(Vec(cfg.vecLen, UInt(cfg.vecDWidth.W)))
    val result = Output(SInt(cfg.resDWidth.W))
  })

  val products = Wire(Vec(cfg.vecLen, SInt((cfg.resDWidth).W)))
  for (i <- 0 until cfg.vecLen) {
    val convertedA = if (cfg.isSignedA) {
      io.vecA(i).asSInt
    } else {
      Cat(0.U(1.W), io.vecA(i)).asSInt
    }
    val convertedB = if (cfg.isSignedB) {
      io.vecB(i).asSInt
    } else {
      Cat(0.U(1.W), io.vecB(i)).asSInt
    }
    products(i) := convertedA * convertedB
  }

  io.result := products.reduce(_ +& _)
}
