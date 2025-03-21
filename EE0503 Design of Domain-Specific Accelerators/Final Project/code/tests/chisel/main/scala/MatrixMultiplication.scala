package GEMM

import chisel3._
import chisel3.util._

class MatrixMultiplicationModule(cfg: MatrixMultiplicationConfigs)
    extends Module {

  assert(cfg.matDWidth == 8, "Only support 8-bit data input width")
  assert(cfg.resDWidth == 32, "Only support 32-bit data output width")
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

    val matARows = Input(UInt(cfg.indexWidth.W))
    val matACols = Input(UInt(cfg.indexWidth.W))
    val matBRows = Input(UInt(cfg.indexWidth.W))
    val matBCols = Input(UInt(cfg.indexWidth.W))

    val busy = Output(Bool())
    val dataOut = Output(SInt(cfg.resDWidth.W))
  })

  /* Padding */
  def pad(size: UInt, coreSize: UInt) = {
    (size + coreSize - 1.U) & ~(coreSize - 1.U(cfg.indexWidth.W))
  }
  val paddedMatARows = pad(io.matARows, cfg.calcCoreSize.U)
  val paddedMatACols = pad(io.matACols, cfg.calcCoreSize.U)
  val paddedMatBRows = pad(io.matBRows, cfg.calcCoreSize.U)
  val paddedMatBCols = pad(io.matBCols, cfg.calcCoreSize.U)

  val blockARow = paddedMatARows / cfg.calcCoreSize.U
  val blockACol = paddedMatACols / cfg.calcCoreSize.U
  val blockBRow = paddedMatBRows / cfg.calcCoreSize.U
  val blockBCol = paddedMatBCols / cfg.calcCoreSize.U
  val blockCRow = paddedMatARows / cfg.calcCoreSize.U
  val blockCCol = paddedMatBCols / cfg.calcCoreSize.U

  val rowAddrDiffA = paddedMatACols * (cfg.matDWidth / 8).U
  val rowAddrDiffB = paddedMatBCols * (cfg.matDWidth / 8).U
  val rowAddrDiffC = paddedMatBCols * (cfg.resDWidth / 8).U

  /* Define submodule */
  val coreModule = Module(
    new MatrixMultiplicationCoreModule(
      MatrixMultiplicationCoreConfigs(
        matSize = cfg.calcCoreSize,
        memSize = cfg.memSize,
        addrWidth = cfg.addrWidth,
        isSignedA = cfg.isSignedA,
        isSignedB = cfg.isSignedB,
        debugEnable = cfg.debugEnable
      )
    )
  )

  /* Address decode */
  val decodedAddress = Wire(UInt(cfg.addrWidth.W))
  when(io.select === cfg.selectA.U) {
    decodedAddress := io.address + ((io.address - cfg.baseAddressA.U) / io.matACols) * (paddedMatACols - io.matACols)
  }.elsewhen(io.select === cfg.selectB.U) {
    decodedAddress := io.address + ((io.address - cfg.baseAddressB.U) / io.matBCols) * (paddedMatBCols - io.matBCols)
  }.otherwise {
    decodedAddress := cfg.baseAddressC.U + (io.address - cfg.baseAddressC.U) * 4.U +
      ((io.address - cfg.baseAddressC.U) / io.matBCols) * (paddedMatBCols - io.matBCols) * 4.U
  }

  /* Define FSM status */
  val ready :: pad :: busy :: Nil = Enum(3)
  val state = RegInit(ready)

  val currentBlockRow = RegInit(0.U(cfg.indexWidth.W))
  val currentBlockCol = RegInit(0.U(cfg.indexWidth.W))
  val index = RegInit(0.U(cfg.indexWidth.W))

  val baseAddressABuffer = RegInit(0.U(cfg.addrWidth.W))
  val baseAddressBBuffer = RegInit(0.U(cfg.addrWidth.W))
  val baseAddressCBuffer = RegInit(0.U(cfg.addrWidth.W))

  val padACol :: padARow :: padBCol :: padBRow :: Nil = Enum(4)
  val padState = RegInit(padACol)
  val padIndex = RegInit(0.U(cfg.indexWidth.W))

  val waitCalculation = RegInit(false.B)
  val waitStore = RegInit(false.B)

  /* Default input & output */
  coreModule.io.dataIn := io.dataIn
  coreModule.io.address := decodedAddress
  coreModule.io.writeEnable := io.writeEnable
  coreModule.io.start := false.B
  coreModule.io.store := false.B
  coreModule.io.baseAddressA := baseAddressABuffer
  coreModule.io.baseAddressB := baseAddressBBuffer
  coreModule.io.baseAddressC := baseAddressCBuffer
  coreModule.io.rowAddrDiffA := rowAddrDiffA
  coreModule.io.rowAddrDiffB := rowAddrDiffB
  coreModule.io.rowAddrDiffC := rowAddrDiffC

  io.busy := (state =/= ready)
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
        when(io.matACols =/= paddedMatACols) {
          state := pad
          padState := padACol
          padIndex := cfg.baseAddressA.U + io.matACols
        }.elsewhen(io.matARows =/= paddedMatARows) {
          state := pad
          padState := padARow
          padIndex := cfg.baseAddressA.U + io.matARows * paddedMatACols
        }.elsewhen(io.matBCols =/= paddedMatBCols) {
          state := pad
          padState := padBCol
          padIndex := cfg.baseAddressB.U + io.matBCols
        }.elsewhen(io.matBRows =/= paddedMatBRows) {
          state := pad
          padState := padBRow
          padIndex := cfg.baseAddressB.U + io.matBRows * paddedMatBCols
        }.otherwise {
          state := busy
        }
      }
    }
    is(pad) {
      /* Pad the input matrix */
      coreModule.io.address := padIndex
      coreModule.io.dataIn := 0.U
      coreModule.io.writeEnable := true.B
      if (cfg.debugEnable) {
        printf("Pad address: %d to 0\n", padIndex)
      }

      switch(padState) {
        is(padACol) {
          when((padIndex - cfg.baseAddressA.U + 1.U) % paddedMatACols === 0.U) {
            when(
              padIndex - cfg.baseAddressA.U + 1.U === io.matARows * paddedMatACols
            ) {
              when(io.matARows =/= paddedMatARows) {
                padState := padARow
                padIndex := cfg.baseAddressA.U + io.matARows * paddedMatACols
              }.elsewhen(io.matBCols =/= paddedMatBCols) {
                padState := padBCol
                padIndex := cfg.baseAddressB.U + io.matBCols
              }.elsewhen(io.matBRows =/= paddedMatBRows) {
                padState := padBRow
                padIndex := cfg.baseAddressB.U + io.matBRows * paddedMatBCols
              }.otherwise {
                state := busy
              }
            }.otherwise {
              padIndex := padIndex + io.matACols + 1.U
            }
          }.otherwise {
            padIndex := padIndex + 1.U
          }
        }
        is(padARow) {
          when(
            padIndex - cfg.baseAddressA.U + 1.U === paddedMatARows * paddedMatACols
          ) {
            when(io.matBCols =/= paddedMatBCols) {
              padState := padBCol
              padIndex := cfg.baseAddressB.U + io.matBCols
            }.elsewhen(io.matBRows =/= paddedMatBRows) {
              padState := padBRow
              padIndex := cfg.baseAddressB.U + io.matBRows * paddedMatBCols
            }.otherwise {
              state := busy
            }
          }.otherwise {
            padIndex := padIndex + 1.U
          }
        }
        is(padBCol) {
          when((padIndex - cfg.baseAddressB.U + 1.U) % paddedMatBCols === 0.U) {
            when(
              padIndex - cfg.baseAddressB.U + 1.U === io.matBRows * paddedMatBCols
            ) {
              when(io.matBRows =/= paddedMatBRows) {
                padState := padBRow
                padIndex := cfg.baseAddressB.U + io.matBRows * paddedMatBCols
              }.otherwise {
                state := busy
              }
            }.otherwise {
              padIndex := padIndex + io.matBCols + 1.U
            }
          }.otherwise {
            padIndex := padIndex + 1.U
          }
        }
        is(padBRow) {
          when(
            padIndex - cfg.baseAddressB.U + 1.U === paddedMatBRows * paddedMatBCols
          ) {
            state := busy
          }.otherwise {
            padIndex := padIndex + 1.U
          }
        }
      }
    }
    is(busy) {
      when(waitStore) {
        when(coreModule.io.finish) {
          waitStore := false.B
          /* Update matrix loop */
          when(currentBlockCol === blockBCol - 1.U) {
            currentBlockCol := 0.U
            when(currentBlockRow === blockARow - 1.U) {
              currentBlockRow := 0.U
              state := ready
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
          when(index === blockACol - 1.U) {
            index := 0.U
            coreModule.io.store := true.B
            waitStore := true.B
          } otherwise {
            index := index + 1.U
          }
        }
      }.otherwise {
        val coreElemCount = cfg.calcCoreSize * cfg.calcCoreSize
        baseAddressABuffer := cfg.baseAddressA.U + currentBlockRow * blockACol * coreElemCount.U + index * cfg.calcCoreSize.U
        baseAddressBBuffer := cfg.baseAddressB.U + index * blockBCol * coreElemCount.U + currentBlockCol * cfg.calcCoreSize.U
        baseAddressCBuffer := cfg.baseAddressC.U + (currentBlockRow * blockCCol * coreElemCount.U + currentBlockCol * cfg.calcCoreSize.U) * 4.U
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
  }

}
