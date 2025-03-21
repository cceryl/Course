package Lab4

import chisel3._
import chiseltest._
import org.scalatest.flatspec.AnyFlatSpec
import scala.util.Random
import java.sql.Driver

class MatrixAccumulatorTest extends AnyFlatSpec with ChiselScalatestTester {

  "MatrixAccumulator test" should "pass" in {

    val testConfig =
      MatrixAccumulatorConfigs(
        matDWidth = 32,
        matSize = 8
      )

    test(
      new MatrixAccumulatorModule(cfg = testConfig)
    ) { dut =>
      for (testID <- 0 until 4) {
        println(s"- Begin test ${testID}")

        /* Generate random matrices */
        val matSize = testConfig.matSize
        val dataWidth = 16
        val cacheDWidth = testConfig.matDWidth
        val matCount = 8
        val mats =
          Seq.fill(matCount)(genRandomMat(matSize, dataWidth, true))
        val matSum = calcMatSum(mats)

        /* Write data to accumulator */
        for (i <- 0 until matCount) {
          for (row <- 0 until matSize) {
            for (col <- 0 until matSize) {
              dut.io.row.poke(row.U)
              dut.io.col.poke(col.U)
              dut.io.dataIn.poke(mats(i)(row)(col).S)
              dut.io.writeEnable.poke(true.B)
              dut.clock.step(1)
            }
          }
        }

        /* Read data from accumulator */
        for (row <- 0 until matSize) {
          for (col <- 0 until matSize) {
            dut.io.row.poke(row.U)
            dut.io.col.poke(col.U)
            dut.io.writeEnable.poke(false.B)
            dut.clock.step(1)
            println(
              s"Mat(${row})(${col}) expected: ${matSum(row)(col)} got: ${dut.io.dataOut.peek()}"
            )
            dut.io.dataOut.expect(matSum(row)(col).S)
          }
        }

        /* Clear accumulator */
        dut.io.clear.poke(true.B)
        dut.clock.step(1)
        dut.io.clear.poke(false.B)

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

  def calcMatSum(mats: Seq[Seq[Seq[BigInt]]]): Seq[Seq[BigInt]] = {
    mats.reduce((a, b) =>
      a.zip(b).map { case (x, y) => x.zip(y).map { case (u, v) => u + v } }
    )
  }
}
