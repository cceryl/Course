package Lab3

import chisel3._
import chiseltest._
import org.scalatest.flatspec.AnyFlatSpec
import scala.util.Random

class MemoryTest extends AnyFlatSpec with ChiselScalatestTester {

  "Memory test" should "pass" in {

    val testConfig = MemoryConfigs(
      addrWidth = 32,
      memSize = 0x1000
    )

    test(new MemoryModule(cfg = testConfig)) { dut =>
      for (testID <- 0 until 10) {
        println(s"- Begin test ${testID}")

        val addressVector = Seq.fill(64)(BigInt(32, scala.util.Random) % 0x1000)
        val dataVector = Seq.fill(64)(BigInt(32, scala.util.Random))
        val sizeVector = Seq.fill(64)(Random.nextInt(3))

        for (i <- 0 until 32) {
          dut.io.address.poke(addressVector(i))
          dut.io.dataIn.poke(dataVector(i))
          dut.io.dataSize.poke(sizeVector(i))
          dut.io.writeEnable.poke(true.B)

          dut.clock.step(1)
          dut.io.writeEnable.poke(false.B)
        }

        dut.clock.step(1)
        val expectedMem =
          generateExpectedMemory(addressVector, dataVector, sizeVector)

        for (i <- 0 until 32) {
          dut.io.address.poke(addressVector(i))
          dut.io.dataSize.poke(sizeVector(i))
          dut.clock.step(1)
          val expectedDataOut =
            readSimulatedMemory(expectedMem, addressVector(i), sizeVector(i))
          dut.io.dataOut.expect(expectedDataOut.U)
          println(
            s"size: ${1 << sizeVector(i)}Byte, dataIn: 0x${dataVector(i)
                .toString(16)}, expected: 0x${expectedDataOut
                .toString(16)}, got: 0x${dut.io.dataOut.peek().litValue.toString(16)}"
          )
        }

        dut.clock.step(1)

        println(s"Test ${testID} passed!")
        dut.clock.step(1)
      }

    }
  }

  def sizeConvert(dataIn: BigInt, sizeSelect: Int): BigInt = {
    if (sizeSelect == 0) {
      dataIn & 0xff
    } else if (sizeSelect == 1) {
      dataIn & 0xffff
    } else if (sizeSelect == 2) {
      dataIn & 0xffffffff
    } else {
      0
    }
  }

  def generateExpectedMemory(
      addrVec: Seq[BigInt],
      dataVec: Seq[BigInt],
      sizeVec: Seq[Int]
  ): Array[BigInt] = {
    val mem = Array.fill(0x1000)(BigInt(8, scala.util.Random))
    for (i <- 0 until 32) {
      val addr = addrVec(i)
      val data = dataVec(i)
      if (sizeVec(i) == 0) {
        mem(addr.toInt) = data & 0xff
      } else if (sizeVec(i) == 1) {
        mem(addr.toInt) = data & 0xff
        mem(addr.toInt + 1) = (data >> 8) & 0xff
      } else if (sizeVec(i) == 2) {
        mem(addr.toInt) = data & 0xff
        mem(addr.toInt + 1) = (data >> 8) & 0xff
        mem(addr.toInt + 2) = (data >> 16) & 0xff
        mem(addr.toInt + 3) = (data >> 24) & 0xff
      }
    }
    mem
  }

  def readSimulatedMemory(
      mem: Array[BigInt],
      addr: BigInt,
      size: Int
  ): BigInt = {
    if (size == 0) {
      mem(addr.toInt)
    } else if (size == 1) {
      (mem(addr.toInt + 1) << 8) + mem(addr.toInt)
    } else if (size == 2) {
      (mem(addr.toInt + 3) << 24) + (mem(addr.toInt + 2) << 16) + (mem(
        addr.toInt + 1
      ) << 8) + mem(addr.toInt)
    } else {
      0
    }
  }

}
