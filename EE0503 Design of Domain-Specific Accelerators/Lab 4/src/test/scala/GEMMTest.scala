package Lab4

import chisel3._
import chiseltest._
import org.scalatest.flatspec.AnyFlatSpec
import scala.util.Random

case class MatMulConfig(
    vecDWidth: Int = 8,
    resDWidth: Int = 32,
    matSize: Int = 4,
    gemm_matsize: Int = 9,
    SEL_A: UInt = 0.U, 
    SEL_B: UInt = 1.U, 
    SEL_C: UInt = 2.U, 
    OP_ACCESS_MEM: UInt = 1.U, 
    OP_COMPUTE: UInt = 0.U
) {

  val gemm_elem_num = gemm_matsize * gemm_matsize
  val memory_size =
    gemm_elem_num * 3
  val matA_baseAddr = 0
  val matB_baseAddr = gemm_elem_num
  val matC_baseAddr = 2 * gemm_elem_num
  val memory_width = resDWidth
}

class GEMMTest extends AnyFlatSpec with ChiselScalatestTester {

  "GeneralMatrixMultiplicationModule test" should "pass" in {
    val test_cfg = MatMulConfig()
    test(new GEMM_TOP(cfg = test_cfg)) { dut =>
      dut.clock.setTimeout(1000000)

      for (testID <- 0 until 4) {
        dut.clock.step(1)
        println(s"Begin test ${testID}")
        // data preparision & poke
        val MatA = generateRandomMatrixUnSigned(
          rows = test_cfg.gemm_matsize,
          cols = test_cfg.gemm_matsize,
          bits = test_cfg.vecDWidth
        )
        val MatB = generateRandomMatrixSigned(
          rows = test_cfg.gemm_matsize,
          cols = test_cfg.gemm_matsize,
          bits = test_cfg.vecDWidth
        )
        println("Calculate MatC.")
        val MatC = matrixMultiplyUnsignedSigned(A = MatA, B = MatB)

        // input A to mem
        println("Loading MatA..")
        dut.io.op.poke(test_cfg.OP_ACCESS_MEM)
        for (row <- 0 until test_cfg.gemm_matsize) {
          println(s"Load MatA row=${row}")
          for (col <- 0 until test_cfg.gemm_matsize) {
            val addr_in_mem =
              test_cfg.matA_baseAddr + row * test_cfg.gemm_matsize + col
            dut.io.dataIn.poke(MatA(row)(col))
            // println(s"MatA(${row})(${col})=${MatA(row)(col)}")
            dut.io.addr.poke(addr_in_mem.U)
            dut.io.writeEn.poke(true.B)
            dut.io.enable.poke(true.B)
            dut.clock.step(1)
          }
        }

        // input B to mem
        println("Loading MatB..")
        for (row <- 0 until test_cfg.gemm_matsize) {
          println(s"Load MatB row=${row}")
          for (col <- 0 until test_cfg.gemm_matsize) {
            val addr_in_mem =
              test_cfg.matB_baseAddr + row * test_cfg.gemm_matsize + col
            // println(s"MatB(${row})(${col})=${MatB(row)(col)}")
            dut.io.dataIn.poke(MatB(row)(col))
            dut.io.addr.poke(addr_in_mem.U)
            dut.io.writeEn.poke(true.B)
            dut.io.enable.poke(true.B)
            dut.clock.step(1)
          }
        }

        dut.io.writeEn.poke(false.B)
        dut.io.enable.poke(false.B)

        // compute
        println("Begin Compute..")
        dut.io.op.poke(test_cfg.OP_COMPUTE)
        dut.io.start.poke(true)
        dut.clock.step(1)
        dut.io.start.poke(false)
        while (dut.io.busy.peekBoolean()) { dut.clock.step(1) }
        println("Complete.")

        // store and check C'res
        dut.io.op.poke(test_cfg.OP_ACCESS_MEM)
        for (row <- 0 until test_cfg.gemm_matsize) {
          for (col <- 0 until test_cfg.gemm_matsize) {
            val addr_in_mem =
              test_cfg.matC_baseAddr + row * test_cfg.gemm_matsize + col
            dut.io.addr.poke(addr_in_mem.U)
            dut.io.writeEn.poke(false.B)
            dut.io.enable.poke(true.B)
            dut.clock.step(1)
            dut.io.dataOut.expect(MatC(row)(col))
            print("ok")
          }
        }
      }

    }
  }

  def generateRandomMatrixSigned(
      rows: Int,
      cols: Int,
      bits: Int
  ): Array[Array[Int]] = {
    val maxValue = math.pow(2, bits - 1).toInt
    Array.fill(rows, cols)((Random.nextInt(maxValue) - maxValue))
  }

  def generateRandomMatrixUnSigned(
      rows: Int,
      cols: Int,
      bits: Int
  ): Array[Array[Int]] = {
    val maxValue = math.pow(2, bits).toInt
    Array.fill(rows, cols)(Random.nextInt(maxValue))
  }

  def matrixMultiplyUnsignedSigned(
      A: Array[Array[Int]],
      B: Array[Array[Int]]
  ): Array[Array[Int]] = {
    require(A(0).length == B.length, "columns of A must equal to rows of B")

    val result = Array.ofDim[Int](A.length, B(0).length)
    for (i <- A.indices) {
      for (j <- B(0).indices) {
        for (k <- A(0).indices) {
          result(i)(j) += A(i)(k) * B(k)(j)
        }
      }
    }
    result
  }

}
