package Lab2

import chisel3._
import chiseltest._
import org.scalatest.flatspec.AnyFlatSpec

class AdderTreeTest extends AnyFlatSpec with ChiselScalatestTester {

  "AdderTree test" should "pass" in {

    val adderTree_numInputs = 8
    val adderTree_width = 16

    test(
      new AdderTree(width = adderTree_width, numInputs = adderTree_numInputs)
    ) { dut =>
      for (testID <- 0 until 10) {
        println(s"Begin test ${testID}")

        var sum: BigInt = BigInt(0)

        for (i <- 0 until adderTree_numInputs) {
          val rand = BigInt(adderTree_width, scala.util.Random)
          sum = sum + rand

          println(s"Random int = 0x${rand.toString(16)}")

          dut.io.inputs(i).poke(rand.U)
        }

        println(s"Sum = 0x${sum.toString(16)}")

        dut.io.output.expect(sum)

        dut.clock.step(1)
      }

    }
  }
}
