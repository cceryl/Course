package Lab4

import chisel3._
import chiseltest._
import org.scalatest.flatspec.AnyFlatSpec
import scala.util.Random
import java.sql.Driver

class MatrixMultiplicationTest extends AnyFlatSpec with ChiselScalatestTester {

  "MatrixMultiplication test" should "pass" in {

    val testConfig = MatrixMultiplicationConfigs(
      matARows = 6,
      matACols = 7,
      matBRows = 7,
      matBCols = 8,
      calcCoreSize = 4,
      memSize = 0x1000,
      addrWidth = 32,
      baseAddressA = 0x0000,
      baseAddressB = 0x0400,
      baseAddressC = 0x0800,
      isSignedA = false,
      isSignedB = true,
      debugEnable = false
    )

    test(
      new MatrixMultiplicationModule(cfg = testConfig)
    ) { dut =>
      dut.clock.setTimeout(1000000)

      for (testID <- 0 until 4) {
        println(s"- Begin test ${testID}")

        /* Generate random matrices */
        val matA = genRandomMat(
          testConfig.matARows,
          testConfig.matACols,
          testConfig.matDWidth,
          testConfig.isSignedA
        )
        val matB = genRandomMat(
          testConfig.matBRows,
          testConfig.matBCols,
          testConfig.matDWidth,
          testConfig.isSignedB
        )
        val result = getTrueProduct(matA, matB)

        /* Input data */
        dut.io.writeEnable.poke(true.B)
        dut.io.select.poke(testConfig.selectA.U)
        for (i <- 0 until testConfig.matARows) {
          for (j <- 0 until testConfig.matACols) {
            dut.io.address.poke(
              (testConfig.baseAddressA + i * testConfig.matACols + j).U
            )
            dut.io.dataIn.poke(
              if (testConfig.isSignedA) asUInt(matA(i)(j), testConfig.matDWidth)
              else matA(i)(j)
            )
            dut.clock.step(1)
          }
        }
        dut.io.select.poke(testConfig.selectB.U)
        for (i <- 0 until testConfig.matBRows) {
          for (j <- 0 until testConfig.matBCols) {
            dut.io.address.poke(
              (testConfig.baseAddressB + i * testConfig.matBCols + j).U
            )
            dut.io.dataIn.poke(
              if (testConfig.isSignedB) asUInt(matB(i)(j), testConfig.matDWidth)
              else matB(i)(j)
            )
            dut.clock.step(1)
          }
        }
        dut.io.writeEnable.poke(false.B)

        /* Check result */
        dut.io.start.poke(true.B)
        dut.clock.step(1)
        dut.io.start.poke(false.B)
        while (!dut.io.finish.peekBoolean()) { dut.clock.step(1) }
        dut.io.select.poke(testConfig.selectC.U)
        for (i <- 0 until testConfig.matARows) {
          for (j <- 0 until testConfig.matBCols) {
            dut.io.address.poke(
              (testConfig.baseAddressC + (i * testConfig.matBCols + j) * 4).U
            )
            dut.clock.step(1)
            if (testConfig.debugEnable) {
              println(
                s"Poking address: ${testConfig.baseAddressC + (i * testConfig.matBCols + j) * 4}"
              )
            }
            println(
              s"MatC(${i})(${j}) expected: ${result(i)(j)}; got: ${dut.io.dataOut.peek()}"
            )
            dut.io.dataOut.expect(result(i)(j).S)
          }
        }

        println(s"Test ${testID} passed!")
        dut.clock.step(1)
      }
    }
  }

  def genRandomMat(
      rows: Int,
      cols: Int,
      dataWidth: Int,
      signed: Boolean
  ): Seq[Seq[BigInt]] = {
    Seq.fill(rows, cols)(
      BigInt(dataWidth, Random) - (if (signed) (1 << (dataWidth - 1)) else 0)
      // BigInt(1)
    )
  }

  def getTrueProduct(
      matA: Seq[Seq[BigInt]],
      matB: Seq[Seq[BigInt]]
  ): Seq[Seq[BigInt]] = {
    var res = Array.fill(matA.length, matB(0).length)(BigInt(0))
    for (i <- 0 until matA.length) {
      for (j <- 0 until matB(0).length) {
        for (k <- 0 until matA(0).length) {
          res(i)(j) += matA(i)(k) * matB(k)(j)
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
