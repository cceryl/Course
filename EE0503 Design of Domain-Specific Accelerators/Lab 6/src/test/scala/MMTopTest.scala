package Lab6

import chisel3._
import chiseltest._
import org.scalatest.flatspec.AnyFlatSpec
import scala.util.Random

class MMTopTest extends AnyFlatSpec with ChiselScalatestTester {

  "MatrixMultiplicationModule test" should "pass" in {
    val testConfig = MatrixMultiplicationTopConfigs(
      matDWidth = 8,
      resDWidth = 32,
      matSize = 16,
      memSize = 0x1000,
      addrWidth = 32,
      baseAddressA = 0x0000,
      baseAddressB = 0x0200,
      baseAddressC = 0x0400,
      accessMemOp = 0,
      computeOp = 1,
      isSignedA = false,
      isSignedB = true
    )

    test(new MMTopModule(cfg = testConfig)) { dut =>
      dut.clock.setTimeout(1000000)

      for (testID <- 0 until 4) {
        dut.clock.step(1)
        println(s"- Begin test ${testID}")

        val MatA = generateRandomMatrixUnSigned(
          rows = testConfig.matSize,
          cols = testConfig.matSize,
          bits = testConfig.matDWidth
        )
        val MatB = generateRandomMatrixSigned(
          rows = testConfig.matSize,
          cols = testConfig.matSize,
          bits = testConfig.matDWidth
        )
        println("Calculating MatC...")
        val MatC = matrixMultiplyUnsignedSigned(A = MatA, B = MatB)

        println("Loading MatA...")
        dut.io.operation.poke(testConfig.accessMemOp)
        for (row <- 0 until testConfig.matSize) {
          for (col <- 0 until testConfig.matSize) {
            val address =
              testConfig.baseAddressA + row * testConfig.matSize + col
            dut.io.dataIn.poke(MatA(row)(col))
            dut.io.address.poke(address.U)
            dut.io.writeEnable.poke(true.B)
            dut.io.enable.poke(true.B)
            dut.clock.step(1)
          }
        }

        println("Loading MatB...")
        for (row <- 0 until testConfig.matSize) {
          for (col <- 0 until testConfig.matSize) {
            val address =
              testConfig.baseAddressB + row * testConfig.matSize + col
            dut.io.dataIn.poke(MatB(row)(col))
            dut.io.address.poke(address.U)
            dut.io.writeEnable.poke(true.B)
            dut.io.enable.poke(true.B)
            dut.clock.step(1)
          }
        }

        dut.io.writeEnable.poke(false.B)
        dut.io.enable.poke(false.B)

        println("Begin Compute...")
        dut.io.operation.poke(testConfig.computeOp)
        dut.io.start.poke(true)
        dut.clock.step(1)
        dut.io.start.poke(false)
        while (dut.io.busy.peekBoolean()) { dut.clock.step(1) }

        println("Checking MatC...")
        dut.io.operation.poke(testConfig.accessMemOp)
        for (row <- 0 until testConfig.matSize) {
          for (col <- 0 until testConfig.matSize) {
            val address =
              testConfig.baseAddressC + (row * testConfig.matSize + col) * 4
            dut.io.address.poke(address.U)
            dut.io.writeEnable.poke(false.B)
            dut.io.enable.poke(true.B)
            dut.clock.step(1)
            dut.io.dataOut.expect(MatC(row)(col))
            println(
              s"MatC(${row})(${col}) expected = ${MatC(row)(col)} got = ${dut.io.dataOut.peek()}"
            )
          }
        }

        println(s"Test ${testID} passed")
      }

    }
  }

  def generateRandomMatrixSigned(
      rows: Int,
      cols: Int,
      bits: Int
  ): Array[Array[Int]] = {
    val maxValue = math.pow(2, bits - 1).toInt
    Array.fill(rows, cols)((Random.nextInt(maxValue) - maxValue / 2))
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
    require(A(0).length == B.length, "A's column must be equal to B's row.")

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