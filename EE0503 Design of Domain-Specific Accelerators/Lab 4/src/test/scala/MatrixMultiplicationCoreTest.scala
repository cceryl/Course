package Lab4

import chisel3._
import chiseltest._
import org.scalatest.flatspec.AnyFlatSpec
import scala.util.Random
import java.sql.Driver

class MatrixMultiplicationCoreTest
    extends AnyFlatSpec
    with ChiselScalatestTester {

  "MatrixMultiplicationCore test" should "pass" in {

    val matSize = 8
    val matCount = 4
    val baseAddressA = 0x0000
    val baseAddressB = 0x0300
    val baseAddressC = 0x0600

    val testConfig =
      MatrixMultiplicationCoreConfigs(
        matSize = matSize,
        memSize = 0x1000,
        addrWidth = 32,
        rowAddrDiffA = matSize + 0x0080,
        rowAddrDiffB = matSize + 0x0080,
        rowAddrDiffC = matSize * 4 + 0x00f0,
        isSignedA = false,
        isSignedB = true
      )

    test(
      new MatrixMultiplicationCoreModule(cfg = testConfig)
    ) { dut =>
      dut.clock.setTimeout(1000000)

      for (testID <- 0 until 4) {
        println(s"- Begin test ${testID}")

        /* Generate random matrices */
        val matSize = testConfig.matSize
        val matsA = Seq.fill(matCount)(
          genRandomMat(matSize, testConfig.matDWidth, testConfig.isSignedA)
        )
        val matsB = Seq.fill(matCount)(
          genRandomMat(matSize, testConfig.matDWidth, testConfig.isSignedB)
        )
        val trueRes = getTrueResult(matsA, matsB, matSize)

        dut.io.baseAddressA.poke(baseAddressA.U)
        dut.io.baseAddressB.poke(baseAddressB.U)
        dut.io.baseAddressC.poke(baseAddressC.U)

        /* Write data to memory */
        for (mat <- 0 until matCount) {
          dut.io.writeEnable.poke(true.B)
          for (i <- 0 until matSize) {
            for (j <- 0 until matSize) {
              dut.io.dataIn.poke(
                if (testConfig.isSignedA)
                  asUInt(matsA(mat)(i)(j), testConfig.matDWidth).U
                else matsA(mat)(i)(j).U
              )
              dut.io.address.poke(
                (baseAddressA + i * testConfig.rowAddrDiffA + j).U
              )
              dut.clock.step(1)
            }
          }
          for (i <- 0 until matSize) {
            for (j <- 0 until matSize) {
              dut.io.dataIn.poke(
                if (testConfig.isSignedB)
                  asUInt(matsB(mat)(i)(j), testConfig.matDWidth).U
                else matsB(mat)(i)(j).U
              )
              dut.io.address.poke(
                (baseAddressB + i * testConfig.rowAddrDiffB + j).U
              )
              dut.clock.step(1)
            }
          }
          dut.io.writeEnable.poke(false.B)
          dut.clock.step(1)

          /* Start the module */
          dut.io.start.poke(true.B)
          dut.clock.step(1)
          dut.io.start.poke(false.B)
          while (!dut.io.finish.peekBoolean()) { dut.clock.step(1) }
        }

        /* Store the result to memory */
        dut.io.store.poke(true.B)
        dut.clock.step(1)
        dut.io.store.poke(false.B)
        while (!dut.io.finish.peekBoolean()) { dut.clock.step(1) }

        /* Read data from memory */
        for (i <- 0 until matSize) {
          for (j <- 0 until matSize) {
            dut.io.address.poke(
              (baseAddressC + i * testConfig.rowAddrDiffC + j * 4).U
            )
            dut.clock.step(1)
            println(
              s"MatC(${i})(${j}) expected: ${trueRes(i)(j)}; got: ${dut.io.dataOut.peek()}"
            )
            dut.io.dataOut.expect(trueRes(i)(j).S)
          }
        }
        println(s"Test ${testID} passed!")
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

  def getTrueResult(
      A: Seq[Seq[Seq[BigInt]]],
      B: Seq[Seq[Seq[BigInt]]],
      matSize: Int
  ): Seq[Seq[BigInt]] = {
    var res = Array.fill(matSize, matSize)(BigInt(0))
    for (matCount <- 0 until A.length) {
      for (i <- 0 until matSize) {
        for (j <- 0 until matSize) {
          for (k <- 0 until matSize) {
            res(i)(j) += A(matCount)(i)(k) * B(matCount)(k)(j)
          }
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
