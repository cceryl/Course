package Lab6_FPGA

import chisel3._
import chisel3.util._
import scala.util.Random

import Lab6._

case class MatMulFunc() {
  def generateRandomMatrixSigned(
      rows: Int,
      cols: Int,
      bits: Int
  ): Array[Array[Int]] = {
    val maxValue = math.pow(2, bits - 1).toInt
    Array.fill(rows, cols)((Random.nextInt(maxValue) - maxValue))
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
    require(A(0).length == B.length, "A's column must equal to B's row")

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

class FPGAShell(configs: MatrixMultiplicationTopConfigs) extends Module {
  val io = IO(new Bundle {
    val start = Input(Bool())
    val check = Input(Bool())
    val finish = Output(Bool())
    val correct = Output(Bool())
  })

  val func = new MatMulFunc
  val MatA = func.generateRandomMatrixUnSigned(
    rows = configs.matSize,
    cols = configs.matSize,
    bits = configs.matDWidth
  )
  val MatB = func.generateRandomMatrixSigned(
    rows = configs.matSize,
    cols = configs.matSize,
    bits = configs.matDWidth
  )
  val MatC = func.matrixMultiplyUnsignedSigned(A = MatA, B = MatB)

  // Create rom data
  val matDataA = VecInit(
    MatA.flatten.toIndexedSeq.map(x => x.S(configs.matDWidth.W))
  )
  // println("MatA:")
  // for (i <- 0 until configs.matSize) {
  //   for (j <- 0 until configs.matSize) {
  //     print(s"${MatA(i)(j)} ")
  //   }
  //   println("")
  // }
  val matDataB = VecInit(
    MatB.flatten.toIndexedSeq.map(x => x.S(configs.matDWidth.W))
  )
  // println("MatB:")
  // for (i <- 0 until configs.matSize) {
  //   for (j <- 0 until configs.matSize) {
  //     print(s"${MatB(i)(j)} ")
  //   }
  //   println("")
  // }

  val matDataC = VecInit(
    MatC.flatten.toIndexedSeq.map(x => x.S(configs.resDWidth.W))
  )
  // println("MatC:")
  // for (i <- 0 until configs.matSize) {
  //   for (j <- 0 until configs.matSize) {
  //     print(s"${MatC(i)(j)} ")
  //   }
  //   println("")
  // }

  // Top module
  val dut = Module(new MMTopModule(configs))

  // Registers
  val correct = RegInit(false.B)
  val finish = RegInit(false.B)
  val memAddr = RegInit(0.U(log2Ceil(configs.memSize).W))
  val romAddr = RegInit(0.U(log2Ceil(configs.memSize).W))

  // Connect
  dut.io.dataIn := 0.S
  dut.io.address := memAddr
  dut.io.writeEnable := false.B
  dut.io.enable := false.B
  dut.io.start := false.B
  io.correct := correct
  io.finish := finish

  // FSM state
  val idle :: initA :: initB :: start :: calculating :: check :: Nil = Enum(6)
  val state = RegInit(idle)

  dut.io.operation := Mux(
    state === initA || state === initB || state === check,
    configs.accessMemOp.U,
    configs.computeOp.U
  )

  val matElementSize = configs.matSize * configs.matSize
  val waitMem = RegInit(true.B)

  switch(state) {
    is(idle) {
      when(io.start) {
        memAddr := configs.baseAddressA.U
        finish := false.B
        correct := false.B
        state := initA
      }.elsewhen(io.check) {
        memAddr := configs.baseAddressC.U
        romAddr := 0.U
        finish := false.B
        correct := false.B
        waitMem := true.B
        state := check
      }
    }
    is(initA) {
      dut.io.dataIn := matDataA(memAddr)
      dut.io.writeEnable := true.B
      dut.io.enable := true.B
      memAddr := memAddr + 1.U
      // printf(
      //   "Writing matDataA value = %d to address = %d\n",
      //   matDataA(memAddr).asUInt,
      //   memAddr
      // )

      when(memAddr === (configs.baseAddressA + matElementSize - 1).U) {
        state := initB
        memAddr := configs.baseAddressB.U
      }
    }
    is(initB) {
      dut.io.dataIn := matDataB(memAddr)
      dut.io.writeEnable := true.B
      dut.io.enable := true.B
      memAddr := memAddr + 1.U
      // printf(
      //   "Writing matDataB value = %d to address = %d\n",
      //   matDataB(memAddr),
      //   memAddr
      // )

      when(memAddr === (configs.baseAddressB + matElementSize - 1).U) {
        state := start
        memAddr := 0.U
      }
    }
    is(start) {
      dut.io.start := true.B
      state := calculating
    }
    is(calculating) {
      finish := Mux(!dut.io.busy, true.B, false.B)
      state := Mux(!dut.io.busy, idle, state)
    }
    is(check) {
      when(waitMem) {
        waitMem := false.B
      }.otherwise {
        waitMem := true.B

        dut.io.enable := true.B
        // printf("Checking matDataC, current address = %d\n", memAddr)
        val wrong = (dut.io.dataOut =/= matDataC(romAddr))
        // printf(
        //   "Reading matDataC value = %d from address = %d\n  The correct value is %d from romAddr = %d\n  The wrong value is %d\n",
        //   dut.io.dataOut,
        //   memAddr,
        //   matDataC(romAddr),
        //   romAddr,
        //   wrong
        // )

        when(wrong) {
          finish := true.B
          correct := false.B
          state := idle
        }.elsewhen(romAddr === (matElementSize - 1).U) {
          finish := true.B
          correct := true.B
          state := idle
        }.otherwise {
          memAddr := memAddr + 4.U
          romAddr := romAddr + 1.U
        }
      }
    }
  }

}
