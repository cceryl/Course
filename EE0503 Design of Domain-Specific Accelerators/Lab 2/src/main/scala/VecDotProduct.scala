package Lab2

import chisel3._
import chisel3.util._

case class ModuleConfigs(
    vecDWidth: Int = 8,
    resDWidth: Int = 32,
    vecLen: Int = 16,
    SEL_A: Bool = true.B,
    SEL_B: Bool = false.B
)

class VecDotProductModule(cfg: ModuleConfigs) extends Module {
  val io = IO(new Bundle {
    val dataIn = Input(UInt(cfg.vecDWidth.W))
    val ld_en = Input(Bool())
    val buf_sel = Input(Bool())
    val dataIn_ptr = Input(UInt(log2Ceil(cfg.vecLen).W))

    val dataOut = Output(UInt(cfg.resDWidth.W))
  })

  val vec_A = RegInit(VecInit(Seq.fill(cfg.vecLen)(0.U(cfg.vecDWidth.W))))
  val vec_B = RegInit(VecInit(Seq.fill(cfg.vecLen)(0.U(cfg.vecDWidth.W))))

  for (i <- 0 until cfg.vecLen) {
    vec_A(i) := Mux(
      io.ld_en && io.buf_sel === cfg.SEL_A && io.dataIn_ptr === i.U,
      io.dataIn,
      vec_A(i)
    )
  }

  for (i <- 0 until cfg.vecLen) {
    vec_B(i) := Mux(
      io.ld_en && io.buf_sel === cfg.SEL_B && io.dataIn_ptr === i.U,
      io.dataIn,
      vec_B(i)
    )
  }

  val productRes = WireInit(VecInit(Seq.fill(cfg.vecLen)(0.U(cfg.resDWidth.W))))
  productRes.zipWithIndex.map { case (product_res, index) =>
    product_res := vec_A(index) * vec_B(index)
  }

  val adderTree = Module(
    new AdderTree(width = cfg.resDWidth, numInputs = cfg.vecLen)
  )
  adderTree.io.inputs := productRes

  io.dataOut := adderTree.io.output
}
