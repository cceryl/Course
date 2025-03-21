package Lab2

import chisel3._
import chiseltest._
import org.scalatest.flatspec.AnyFlatSpec

/* This is modified for debugging */
class FSMVecDotProductTest extends AnyFlatSpec with ChiselScalatestTester {

  "FSMVecDotProduct test" should "pass" in {
    val test_cfg = vecDotFSM_Configs()
    test(new FSM_VecDotProductModule(cfg = test_cfg)) { dut =>
      for (testID <- 0 until 5) {
        dut.clock.step(1)
        println(s"- Begin test ${testID}")

        val true_res_array: Array[BigInt] =
          Array.fill(test_cfg.vecDepth)(BigInt(0))
        for (depth <- 0 until test_cfg.vecDepth) {
          val vec_A = gen_randomVec(
            nElem = test_cfg.vecLen,
            elemWidth = test_cfg.vecDWidth
          )
          val vec_B = gen_randomVec(
            nElem = test_cfg.vecLen,
            elemWidth = test_cfg.vecDWidth
          )
          val rand_d = BigInt(test_cfg.vecDWidth, scala.util.Random)
          val true_dot_product_res = get_true_mac(
            vec_A = vec_A,
            vec_B = vec_B,
            d = rand_d,
            vecLen = test_cfg.vecLen
          )
          true_res_array(depth) = true_dot_product_res

          dut.io.ld_en.poke(true)
          dut.io.buf_sel.poke(test_cfg.SEL_A.U)
          var input_ptr = 0
          for (elem <- 0 until test_cfg.vecLen) {
            dut.io.dataIn.poke(vec_A(elem).U)
            dut.io.dept_ptr.poke(depth.U)
            dut.io.elem_ptr.poke(elem.U)
            dut.clock.step(1)
          }

          dut.io.ld_en.poke(true)
          dut.io.buf_sel.poke(test_cfg.SEL_B.U)
          input_ptr = 0
          for (elem <- 0 until test_cfg.vecLen) {
            dut.io.dataIn.poke(vec_B(elem).U)
            dut.io.dept_ptr.poke(depth.U)
            dut.io.elem_ptr.poke(elem.U)
            dut.clock.step(1)
          }

          dut.io.ld_en.poke(true)
          dut.io.buf_sel.poke(test_cfg.SEL_D.U)
          dut.io.dept_ptr.poke(depth.U)
          dut.io.dataIn.poke(rand_d.U)
          dut.clock.step(1)

          dut.io.ld_en.poke(true)
        }

        dut.io.start.poke(true)
        dut.clock.step(1)
        dut.io.start.poke(false)

        for (depth <- 0 until test_cfg.vecDepth) {
          while (!dut.io.finish.peekBoolean()) { dut.clock.step(1) }
          println(
            s"depth=${depth} expected=${true_res_array(depth)} got=${dut.io.dataOut.peek()}"
          )
          dut.io.dataOut.expect(true_res_array(depth).U)
          dut.clock.step(1)
        }
        println(s"Test ${testID} passed!")
        dut.clock.step(1)
      }
    }
  }

  def gen_randomVec(nElem: Int, elemWidth: Int): Array[BigInt] = {
    val dataArray = Array.ofDim[BigInt](nElem)

    for (i <- 0 until nElem) {
      dataArray(i) = BigInt(elemWidth, scala.util.Random)
    }

    return dataArray
  }

  def get_true_mac(
      vec_A: Array[BigInt],
      vec_B: Array[BigInt],
      d: BigInt,
      vecLen: Int
  ): BigInt = {
    var trueSum: BigInt = BigInt(0)
    for (elem <- 0 until vecLen) {
      trueSum = trueSum + vec_A(elem) * vec_B(elem)
    }
    return trueSum + d
  }
}
