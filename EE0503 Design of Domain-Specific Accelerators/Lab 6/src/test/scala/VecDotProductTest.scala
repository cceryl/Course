package Lab6

import chisel3._
import chiseltest._
import org.scalatest.flatspec.AnyFlatSpec

class VecDotProductTest extends AnyFlatSpec with ChiselScalatestTester {

  "VecDotProduct test" should "pass" in {

    val testConfig = VecDotProductConfigs(
      vecDWidth = 8,
      resDWidth = 32,
      vecLen = 16,
      isSignedA = false,
      isSignedB = true
    )

    test(new VecDotProductModule(cfg = testConfig)) { dut =>
      for (testID <- 0 until 8) {
        dut.clock.step(1)
        println(s"- Begin test ${testID}")

        val vecA =
          genRandomVec(
            vecLen = testConfig.vecLen,
            elemWidth = testConfig.vecDWidth,
            signed = testConfig.isSignedA
          )
        val vecB =
          genRandomVec(
            vecLen = testConfig.vecLen,
            elemWidth = testConfig.vecDWidth,
            signed = testConfig.isSignedB
          )
        val trueResult =
          getTrueResult(vecA = vecA, vecB = vecB, vecLen = testConfig.vecLen)

        for (i <- 0 until testConfig.vecLen) {
          dut.io
            .vecA(i)
            .poke(if (testConfig.isSignedA) asUInt(vecA(i)).U else vecA(i).U)
          dut.io
            .vecB(i)
            .poke(if (testConfig.isSignedB) asUInt(vecB(i)).U else vecB(i).U)
        }
        println(
          s"Product: ${trueResult}, got: ${dut.io.result.peek()}"
        )
        dut.io.result.expect(trueResult.S)

        println(s"Test ${testID} passed!")
        dut.clock.step(1)
      }

    }
  }

  def genRandomVec(
      vecLen: Int,
      elemWidth: Int,
      signed: Boolean
  ): Array[BigInt] = {
    val dataArray = Array.ofDim[BigInt](vecLen)

    for (i <- 0 until vecLen) {
      if (signed) {
        dataArray(i) =
          BigInt(elemWidth, scala.util.Random) - (BigInt(1) << (elemWidth - 1))
      } else
        dataArray(i) = BigInt(elemWidth, scala.util.Random)
    }

    return dataArray
  }

  def getTrueResult(
      vecA: Array[BigInt],
      vecB: Array[BigInt],
      vecLen: Int
  ): BigInt = {
    var trueSum: BigInt = BigInt(0)
    for (elem <- 0 until vecLen) {
      trueSum = trueSum + vecA(elem) * vecB(elem)
    }
    return trueSum
  }

  def asUInt(x: BigInt): BigInt = {
    if (x < 0) {
      x + (BigInt(1) << 8)
    } else {
      x
    }
  }
}
