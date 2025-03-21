package Adder

import chisel3._
import chiseltest._
import org.scalatest.flatspec.AnyFlatSpec

class AdderTest extends AnyFlatSpec with ChiselScalatestTester {

  "Adder test" should "pass" in {

    test(new Uint64_Adder) { dut =>
      for (testID <- 0 until 10) {
        println(s"Begin test ${testID}")

        val a = scala.util.Random.nextInt(0xffff)
        val b = scala.util.Random.nextInt(0xffff)
        val c = a + b
        val carry = 0

        println(
          s"a=0x${a.toHexString} b=0x${b.toHexString} c=0x${c.toHexString} carry=${carry}"
        )

        dut.io.in_a.poke(a.U)
        dut.io.in_b.poke(b.U)
        dut.io.out_sum.expect(c.U)
        dut.io.out_carry.expect(carry.B)

        dut.clock.step(1)
      }
    }

    test(new Uint64_Adder) { dut =>
      for (testID <- 10 until 20) {
        println(s"Begin test ${testID}")

        val a = BigInt(64, scala.util.Random)
        val b = BigInt(64, scala.util.Random)
        val c = (a + b) & BigInt("FFFFFFFFFFFFFFFF", 16)
        val carry = ((a + b) >> 64) & 1

        println(
          s"a=0x${a.toString(16)} b=0x${b.toString(16)} c=0x${c.toString(16)} carry=${carry}"
        )

        dut.io.in_a.poke(a.U)
        dut.io.in_b.poke(b.U)
        dut.io.out_sum.expect(c.U)
        dut.io.out_carry.expect(carry.B)

        dut.clock.step(1)
      }
    }
  }

}
