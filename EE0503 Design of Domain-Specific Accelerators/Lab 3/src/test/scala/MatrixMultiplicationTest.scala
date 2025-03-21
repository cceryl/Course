package Lab3

import chisel3._
import chiseltest._
import org.scalatest.flatspec.AnyFlatSpec
import scala.util.Random
import java.sql.Driver

class MatrixMultiplicationTest extends AnyFlatSpec with ChiselScalatestTester {

  "Memory test" should "pass" in {

    val testConfig =
      MatrixMultiplicationConfigs(
        matSize = 16,
        memSize = 0x1000,
        addrWidth = 32,
        baseAddressA = 0x0000,
        baseAddressB = 0x0200,
        baseAddressC = 0x0400,
        isSignedA = false,
        isSignedB = true
      )

    test(
      new MatrixMultiplicationModule(cfg = testConfig)
    ) { dut =>
      dut.clock.setTimeout(1000000)

      for (testID <- 0 until 4) {
        println(s"- Begin test ${testID}")

        /* Generate random matrix */
        val matSize = testConfig.matSize
        val matA =
          genRandomMat(matSize, testConfig.matDWidth, testConfig.isSignedA)
        val matB =
          genRandomMat(matSize, testConfig.matDWidth, testConfig.isSignedB)
        val trueRes = calcTrueMatMul(matA, matB, matSize)

        /* Write data to memory */
        dut.io.writeEnable.poke(true.B)
        for (i <- 0 until matSize) {
          for (j <- 0 until matSize) {
            dut.io.dataIn.poke(
              if (testConfig.isSignedA)
                asUInt(matA(i)(j), testConfig.matDWidth).U
              else matA(i)(j).U
            )
            dut.io.address
              .poke((testConfig.baseAddressA + i * matSize + j).U)
            dut.clock.step(1)
          }
        }
        for (i <- 0 until matSize) {
          for (j <- 0 until matSize) {
            dut.io.dataIn.poke(
              if (testConfig.isSignedB)
                asUInt(matB(i)(j), testConfig.matDWidth).U
              else matB(i)(j).U
            )
            dut.io.address
              .poke((testConfig.baseAddressB + i * matSize + j).U)
            dut.clock.step(1)
          }
        }
        dut.io.writeEnable.poke(false.B)

        /* Start the module */
        dut.io.start.poke(true.B)
        dut.clock.step(1)
        dut.io.start.poke(false.B)
        while (!dut.io.finish.peekBoolean()) { dut.clock.step(1) }

        /* Read data from memory */
        for (i <- 0 until matSize) {
          for (j <- 0 until matSize) {
            dut.io.address
              .poke((testConfig.baseAddressC + (i * matSize + j) * 4).U)
            dut.clock.step(1)
            println(
              s"MatC(${i})(${j}) expected: ${trueRes(i)(j)}; got: ${dut.io.dataOut.peek()}"
            )
            dut.io.dataOut.expect(trueRes(i)(j).S)
          }
        }

        println(s"Test ${testID} passed!")
        dut.clock.step(1)
      }

    }
  }

  def genRandomMat(
      matSize: Int,
      dataWidth: Int,
      isSigned: Boolean
  ): Seq[Seq[BigInt]] = {
    val mat = Seq.fill(matSize, matSize)(
      BigInt(dataWidth, scala.util.Random) -
        (if (isSigned) (BigInt(1) << (dataWidth - 1)) else 0)
    )
    mat
  }

  def calcTrueMatMul(
      A: Seq[Seq[BigInt]],
      B: Seq[Seq[BigInt]],
      matSize: Int
  ): Seq[Seq[BigInt]] = {
    val res = Array.ofDim[BigInt](matSize, matSize)
    for (i <- 0 until matSize) {
      for (j <- 0 until matSize) {
        res(i)(j) = 0
        for (k <- 0 until matSize) {
          res(i)(j) += A(i)(k) * B(k)(j)
        }
      }
    }
    res.map(_.toSeq).toSeq
  }

  def asUInt(x: BigInt, width: Int): BigInt = {
    if (x < 0) {
      (BigInt(1) << width) + x
    } else {
      x
    }
  }
}
