package GEMM

import chisel3._
import chisel3.util._

import ROCC_Lab_config._

case class MatrixAccumulatorConfigs(
    /* Configurable */
    matDWidth: Int = 8,
    resDWidth: Int = 32,
    matSize: Int = 16
)

class MatrixAccumulatorModule(cfg: MatrixAccumulatorConfigs) extends Module {

  val io = IO(new Bundle {
    val row = Input(UInt(log2Ceil(cfg.matSize).W))
    val col = Input(UInt(log2Ceil(cfg.matSize).W))
    val dataIn = Input(SInt(cfg.matDWidth.W))
    val writeEnable = Input(Bool())
    val clear = Input(Bool())

    val dataOut = Output(SInt(cfg.resDWidth.W))
  })

  val accumulator = RegInit(
    VecInit(Seq.fill(cfg.matSize * cfg.matSize)(0.S(cfg.resDWidth.W)))
  )

  when(io.writeEnable) {
    val index = io.row * cfg.matSize.U + io.col
    accumulator(index) := accumulator(index) + io.dataIn
  }

  when(io.clear) {
    accumulator.foreach { _ := 0.S }
  }

  io.dataOut := accumulator(io.row * cfg.matSize.U + io.col)
}
