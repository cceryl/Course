package Lab6_FPGA

import chisel3._
import chiseltest._
import org.scalatest.flatspec.AnyFlatSpec
import scala.util.Random

import Lab6._

class FPGAShellTest extends AnyFlatSpec with ChiselScalatestTester {

  "FPGAShell test" should "pass" in {
    val testConfig = MatrixMultiplicationTopConfigs(
      matDWidth = 8,
      resDWidth = 32,
      matSize = 8,
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

    test(new FPGAShell(configs = testConfig)) { dut =>
      dut.clock.setTimeout(1000000)

      dut.io.start.poke(false.B)
      dut.io.check.poke(false.B)

      for (testID <- 0 until 4) {
        dut.clock.step(1)
        println(s"- Begin test ${testID}")

        dut.io.start.poke(true.B)
        dut.clock.step(1)
        dut.io.start.poke(false.B)

        while (!dut.io.finish.peek().litToBoolean) {
          dut.clock.step(1)
        }
        dut.clock.step(1)
        println(s"Finish calculation")

        dut.io.check.poke(true.B)
        dut.clock.step(1)
        dut.io.check.poke(false.B)

        while(!dut.io.finish.peek().litToBoolean) {
          dut.clock.step(1)
        }
        dut.io.correct.expect(true.B)

        println(s"Test ${testID} passed")
      }

    }
  }

}
