package Lab2

import chisel3._
import chisel3.util._

case class vecDotFSM_Configs(
    /* Bit size of vector elements */
    vecDWidth: Int = 8,
    /* Bit size of result */
    resDWidth: Int = 32,
    /* Length of vector */
    vecLen: Int = 16,
    /* Depth of vector buffer */
    vecDepth: Int = 4,
    /* Select vector A */
    SEL_A: Int = 0,
    /* Select vector B */
    SEL_B: Int = 1,
    /* Select bias */
    SEL_D: Int = 2
)

class FSM_VecDotProductModule(cfg: vecDotFSM_Configs) extends Module {

  /* Define IO */
  val io = IO(new Bundle {
    /* Input element */
    val dataIn = Input(UInt(cfg.vecDWidth.W))
    /* Load enable for an element */
    val ld_en = Input(Bool())
    /* Selector of vector A buffer, vector B buffer or bias buffer */
    val buf_sel = Input(UInt(log2Ceil(3).W))
    /* Selector of depth */
    val dept_ptr = Input(UInt(log2Ceil(cfg.vecDepth).W))
    /* Selector of element */
    val elem_ptr = Input(UInt(log2Ceil(cfg.vecLen).W))
    /* Start signal */
    val start = Input(Bool())

    /* Result */
    val dataOut = Output(UInt(cfg.resDWidth.W))
    /* Finish signal */
    val finish = Output(Bool())
  })

  /* Define input vector, bias and output buffers */
  val A_buf = RegInit(
    VecInit(
      Seq.fill(cfg.vecDepth)(
        VecInit(Seq.fill(cfg.vecLen)(0.U(cfg.vecDWidth.W)))
      )
    )
  )
  val B_buf = RegInit(
    VecInit(
      Seq.fill(cfg.vecDepth)(
        VecInit(Seq.fill(cfg.vecLen)(0.U(cfg.vecDWidth.W)))
      )
    )
  )
  val d_buf = RegInit(
    VecInit(Seq.fill(cfg.vecDepth)(0.U(cfg.vecDWidth.W)))
  )
  val out_buf = RegInit(0.U(cfg.resDWidth.W))

  /* Data input */
  for (depth <- 0 until cfg.vecDepth) {
    for (elem <- 0 until cfg.vecLen) {
      val can_load =
        io.ld_en && io.buf_sel === cfg.SEL_A.U && io.dept_ptr === depth.U && io.elem_ptr === elem.U
      A_buf(depth)(elem) := Mux(
        can_load,
        io.dataIn,
        A_buf(depth)(elem)
      )
    }
  }
  for (depth <- 0 until cfg.vecDepth) {
    for (elem <- 0 until cfg.vecLen) {
      val can_load =
        io.ld_en && io.buf_sel === cfg.SEL_B.U && io.dept_ptr === depth.U && io.elem_ptr === elem.U
      B_buf(depth)(elem) := Mux(
        can_load,
        io.dataIn,
        B_buf(depth)(elem)
      )
    }
  }
  for (depth <- 0 until cfg.vecDepth) {
    val can_load =
      io.ld_en && io.buf_sel === cfg.SEL_D.U && io.dept_ptr === depth.U
    d_buf(depth) := Mux(
      can_load,
      io.dataIn,
      d_buf(depth)
    )
  }

  /* Define FSM status */
  val ready :: execute :: finish :: Nil = Enum(3)
  val state = RegInit(ready)
  val counter = RegInit(0.U(log2Ceil(cfg.vecDepth).W))

  /* Default output */
  io.dataOut := out_buf
  io.finish := false.B

  /* FSM */
  switch(state) {
    is(ready) {

      /* Wait for start signal */
      when(io.start) {
        state := execute
      }

    }
    is(execute) {

      /* Dot product & adder tree */
      val dot_product_results = WireInit(
        VecInit(Seq.fill(cfg.vecLen + 1)(0.U(cfg.resDWidth.W)))
      )

      for (index <- 0 until cfg.vecLen) {
        dot_product_results(index) := A_buf(0)(index) * B_buf(0)(index)
      }
      dot_product_results(cfg.vecLen) := d_buf(0)

      val adderTree = Module(
        new AdderTree(width = cfg.resDWidth, numInputs = cfg.vecLen + 1)
      )
      adderTree.io.inputs := dot_product_results

      /* Transfer data between depths */
      for (depth <- 0 until cfg.vecDepth - 1) {
        for (index <- 0 until cfg.vecLen) {
          A_buf(depth)(index) := A_buf(depth + 1)(index)
          B_buf(depth)(index) := B_buf(depth + 1)(index)
        }
        d_buf(depth) := d_buf(depth + 1)
      }

      /* Set output buffer */
      out_buf := adderTree.io.output
      state := finish
    }
    is(finish) {

      /* Send finish signal */
      io.finish := true.B

      /* Check depth loop */
      when(counter === (cfg.vecDepth - 1).U) {
        counter := 0.U
        state := ready
      } otherwise {
        counter := counter + 1.U
        state := execute
      }

    }
  }
}
