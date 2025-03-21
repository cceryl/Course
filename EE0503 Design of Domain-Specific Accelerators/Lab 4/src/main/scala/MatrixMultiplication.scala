package Lab4

import chisel3._
import chisel3.util._

case class MatrixMultiplicationConfigs(
    /* Fixed */
    matDWidth: Int = 8,
    resDWidth: Int = 32,
    /* Configurable */
    matARows: Int = 16,
    matACols: Int = 16,
    matBRows: Int = 16,
    matBCols: Int = 16,
    calcCoreSize: Int = 8,
    memSize: Int = 0x1000,
    addrWidth: Int = 32,
    baseAddressA: Int = 0x0000,
    baseAddressB: Int = 0x0400,
    baseAddressC: Int = 0x0800,
    isSignedA: Boolean = false,
    isSignedB: Boolean = false,
    debugEnable: Boolean = false,
    /* Enum */
    selectA: Int = 0,
    selectB: Int = 1,
    selectC: Int = 2,
    byteOp: Int = 0,
    halfOp: Int = 1,
    wordOp: Int = 2
)

class MatrixMultiplicationModule(cfg: MatrixMultiplicationConfigs)
    extends Module {

  assert(cfg.matDWidth == 8, "Only support 8-bit data input width")
  assert(cfg.resDWidth == 32, "Only support 32-bit data output width")
  assert(
    cfg.matACols == cfg.matBRows,
    "Matrix A columns must be equal to Matrix B rows"
  )
  assert(
    cfg.matARows * cfg.matACols + cfg.matBRows * cfg.matBCols + cfg.matARows * cfg.matBCols * 4 <= cfg.memSize,
    "Memory size is not enough for storing all matrices"
  )
  assert(
    (cfg.calcCoreSize & (cfg.calcCoreSize - 1)) == 0,
    "Calculation core size must be power of 2"
  )

  /* Define I/O */
  val io = IO(new Bundle {
    val start = Input(Bool())
    val dataIn = Input(UInt(cfg.matDWidth.W))
    val address = Input(UInt(cfg.addrWidth.W))
    val select = Input(UInt(2.W))
    val writeEnable = Input(Bool())

    val finish = Output(Bool())
    val dataOut = Output(SInt(cfg.resDWidth.W))
  })

  /* Padding */
  val paddedMatARows =
    (cfg.matARows + cfg.calcCoreSize - 1) & ~(cfg.calcCoreSize - 1)
  val paddedMatACols =
    (cfg.matACols + cfg.calcCoreSize - 1) & ~(cfg.calcCoreSize - 1)
  val paddedMatBRows =
    (cfg.matBRows + cfg.calcCoreSize - 1) & ~(cfg.calcCoreSize - 1)
  val paddedMatBCols =
    (cfg.matBCols + cfg.calcCoreSize - 1) & ~(cfg.calcCoreSize - 1)

  val blockARow = paddedMatARows / cfg.calcCoreSize
  val blockACol = paddedMatACols / cfg.calcCoreSize
  val blockBRow = paddedMatBRows / cfg.calcCoreSize
  val blockBCol = paddedMatBCols / cfg.calcCoreSize
  val blockCRow = paddedMatARows / cfg.calcCoreSize
  val blockCCol = paddedMatBCols / cfg.calcCoreSize

  val addrToPad = Seq(
    for (i <- 0 until cfg.matARows)
      yield (cfg.baseAddressA + i * paddedMatACols + cfg.matACols until cfg.baseAddressA + (i + 1) * paddedMatACols),
    for (i <- cfg.matARows until paddedMatARows)
      yield (cfg.baseAddressA + i * paddedMatACols until cfg.baseAddressA + (i + 1) * paddedMatACols),
    for (i <- 0 until cfg.matBRows)
      yield (cfg.baseAddressB + i * paddedMatBCols + cfg.matBCols until cfg.baseAddressB + (i + 1) * paddedMatBCols),
    for (i <- cfg.matBRows until paddedMatBRows)
      yield (cfg.baseAddressB + i * paddedMatBCols until cfg.baseAddressB + (i + 1) * paddedMatBCols)
  ).flatten.flatten.map(_.U)
  val addrToPadBuffer =
    if (addrToPad.isEmpty) VecInit(cfg.memSize.U) else VecInit(addrToPad)

  /* Define submodule */
  val coreModule = Module(
    new MatrixMultiplicationCoreModule(
      MatrixMultiplicationCoreConfigs(
        matSize = cfg.calcCoreSize,
        memSize = cfg.memSize,
        addrWidth = cfg.addrWidth,
        rowAddrDiffA = paddedMatACols * (cfg.matDWidth / 8),
        rowAddrDiffB = paddedMatBCols * (cfg.matDWidth / 8),
        rowAddrDiffC = paddedMatBCols * (cfg.resDWidth / 8),
        isSignedA = cfg.isSignedA,
        isSignedB = cfg.isSignedB,
        debugEnable = cfg.debugEnable
      )
    )
  )

  /* Address decode */
  val decodedAddress = Wire(UInt(cfg.addrWidth.W))
  when(io.select === cfg.selectA.U) {
    decodedAddress := io.address + ((io.address - cfg.baseAddressA.U) / cfg.matACols.U) * (paddedMatACols - cfg.matACols).U
  }.elsewhen(io.select === cfg.selectB.U) {
    decodedAddress := io.address + ((io.address - cfg.baseAddressB.U) / cfg.matBCols.U) * (paddedMatBCols - cfg.matBCols).U
  }.otherwise {
    decodedAddress := io.address + ((io.address - cfg.baseAddressC.U) / 4.U / cfg.matBCols.U) * (paddedMatBCols - cfg.matBCols).U * 4.U
  }

  /* Define FSM status */
  val ready :: pad :: busy :: finish :: Nil = Enum(4)
  val state = RegInit(ready)

  val currentBlockRow = RegInit(0.U(log2Ceil(blockARow).W))
  val currentBlockCol = RegInit(0.U(log2Ceil(blockBCol).W))
  val index = RegInit(0.U(log2Ceil(blockACol).W))

  val baseAddressABuffer = RegInit(0.U(cfg.addrWidth.W))
  val baseAddressBBuffer = RegInit(0.U(cfg.addrWidth.W))
  val baseAddressCBuffer = RegInit(0.U(cfg.addrWidth.W))

  val padIndex = RegInit(0.U(log2Ceil(addrToPadBuffer.length).W))

  val waitCalculation = RegInit(false.B)
  val waitStore = RegInit(false.B)

  /* Default input & output */
  coreModule.io.dataIn := io.dataIn
  coreModule.io.address := decodedAddress
  coreModule.io.writeEnable := io.writeEnable
  coreModule.io.baseAddressA := baseAddressABuffer
  coreModule.io.baseAddressB := baseAddressBBuffer
  coreModule.io.baseAddressC := baseAddressCBuffer
  coreModule.io.start := false.B
  coreModule.io.store := false.B
  io.finish := false.B
  io.dataOut := coreModule.io.dataOut
  if (cfg.debugEnable) {
    when(io.writeEnable) {
      printf("Write data %d to address %d\n", io.dataIn, decodedAddress)
    }
  }

  /* FSM */
  switch(state) {
    is(ready) {
      /* Wait for start signal */
      when(io.start) {
        when(addrToPadBuffer(0) === cfg.memSize.U) {
          state := busy
        }.otherwise {
          state := pad
        }
      }
    }
    is(pad) {
      /* Pad the input matrix */
      coreModule.io.address := addrToPadBuffer(padIndex)
      coreModule.io.dataIn := 0.U
      coreModule.io.writeEnable := true.B
      if (cfg.debugEnable) {
        printf("Pad address: %d as 0\n", addrToPadBuffer(padIndex))
      }
      when(padIndex === (addrToPadBuffer.length - 1).U) {
        padIndex := 0.U
        state := busy
      } otherwise {
        padIndex := padIndex + 1.U
      }
    }
    is(busy) {
      when(waitStore) {
        when(coreModule.io.finish) {
          waitStore := false.B
          /* Update matrix loop */
          when(currentBlockCol === (blockBCol - 1).U) {
            currentBlockCol := 0.U
            when(currentBlockRow === (blockARow - 1).U) {
              currentBlockRow := 0.U
              state := finish
            } otherwise {
              currentBlockRow := currentBlockRow + 1.U
            }
          } otherwise {
            currentBlockCol := currentBlockCol + 1.U
          }
        }
      }.elsewhen(waitCalculation) {
        when(coreModule.io.finish) {
          waitCalculation := false.B
          /* Update index loop */
          when(index === (blockACol - 1).U) {
            index := 0.U
            coreModule.io.store := true.B
            waitStore := true.B
          } otherwise {
            index := index + 1.U
          }
        }
      }.otherwise {
        val coreElemCount = cfg.calcCoreSize * cfg.calcCoreSize
        baseAddressABuffer := cfg.baseAddressA.U + currentBlockRow * blockACol.U * coreElemCount.U + index * cfg.calcCoreSize.U
        baseAddressBBuffer := cfg.baseAddressB.U + index * blockBCol.U * coreElemCount.U + currentBlockCol * cfg.calcCoreSize.U
        baseAddressCBuffer := cfg.baseAddressC.U + (currentBlockRow * blockCCol.U * coreElemCount.U + currentBlockCol * cfg.calcCoreSize.U) * 4.U
        coreModule.io.start := true.B
        waitCalculation := true.B
        if (cfg.debugEnable) {
          printf(
            "Start calculation at block A(%d, %d) and block B(%d, %d)\n",
            currentBlockRow,
            index,
            index,
            currentBlockCol
          )
        }
      }
    }
    is(finish) {
      /* Send finish signal */
      io.finish := true.B
      state := ready
    }
  }

}
