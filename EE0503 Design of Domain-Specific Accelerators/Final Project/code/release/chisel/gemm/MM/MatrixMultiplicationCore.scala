package GEMM

import chisel3._
import chisel3.util._

import ROCC_Lab_config._

case class MatrixMultiplicationCoreConfigs(
    /* Fixed */
    matDWidth: Int = 8,
    resDWidth: Int = 32,
    /* Configurable */
    matSize: Int = 16,
    memSize: Int = 0x1000,
    addrWidth: Int = 32,
    isSignedA: Boolean = false,
    isSignedB: Boolean = false,
    debugEnable: Boolean = false,
    /* Enum */
    selectA: Int = 0,
    selectB: Int = 1,
    byteOp: Int = 0,
    halfOp: Int = 1,
    wordOp: Int = 2
)

class MatrixMultiplicationCoreModule(cfg: MatrixMultiplicationCoreConfigs)
    extends Module {

  assert(cfg.matDWidth == 8, "Only support 8-bit data input width")
  assert(cfg.resDWidth == 32, "Only support 32-bit data output width")

  /* Define I/O */
  val io = IO(new Bundle {
    val dataIn = Input(UInt(cfg.matDWidth.W))
    val address = Input(UInt(cfg.addrWidth.W))
    val writeEnable = Input(Bool())
    val start = Input(Bool())
    val store = Input(Bool())

    val baseAddressA = Input(UInt(cfg.addrWidth.W))
    val baseAddressB = Input(UInt(cfg.addrWidth.W))
    val baseAddressC = Input(UInt(cfg.addrWidth.W))
    val rowAddrDiffA = Input(UInt(cfg.addrWidth.W))
    val rowAddrDiffB = Input(UInt(cfg.addrWidth.W))
    val rowAddrDiffC = Input(UInt(cfg.addrWidth.W))

    val finish = Output(Bool())
    val dataOut = Output(SInt(cfg.resDWidth.W))
  })

  /* Define vector buffers */
  val bufferRow = RegInit(VecInit(Seq.fill(cfg.matSize)(0.U(cfg.matDWidth.W))))
  val bufferCol = RegInit(VecInit(Seq.fill(cfg.matSize)(0.U(cfg.matDWidth.W))))

  /* Define submodules */
  val memoryModule = Module(
    new MemoryModule(
      MemoryConfigs(
        addrWidth = cfg.addrWidth,
        memSize = cfg.memSize
      )
    )
  )
  val vecDotProductModule = Module(
    new VectorDotProductModule(
      VectorDotProductConfigs(
        vecDWidth = cfg.matDWidth,
        resDWidth = cfg.resDWidth,
        vecLen = cfg.matSize,
        isSignedA = cfg.isSignedA,
        isSignedB = cfg.isSignedB
      )
    )
  )
  val accumulatorModule = Module(
    new MatrixAccumulatorModule(
      MatrixAccumulatorConfigs(
        matDWidth = cfg.resDWidth,
        resDWidth = cfg.resDWidth,
        matSize = cfg.matSize
      )
    )
  )

  /* Define FSM status */
  val ready :: load :: busy :: finish :: store :: Nil = Enum(5)
  val state = RegInit(ready)
  val currentRow = RegInit(0.U(log2Ceil(cfg.matSize).W))
  val currentCol = RegInit(0.U(log2Ceil(cfg.matSize).W))
  val loadRow :: loadCol :: Nil = Enum(2)
  val loadState = RegInit(loadRow)
  val index = RegInit(0.U(log2Ceil(cfg.matSize).W))
  val waitMem = RegInit(true.B)

  /* Default input & output */
  memoryModule.io.dataIn := io.dataIn
  memoryModule.io.address := io.address
  memoryModule.io.dataSize := Mux(io.writeEnable, cfg.byteOp.U, cfg.wordOp.U)
  memoryModule.io.writeEnable := io.writeEnable
  vecDotProductModule.io.vecA := bufferRow
  vecDotProductModule.io.vecB := bufferCol
  accumulatorModule.io.row := currentRow
  accumulatorModule.io.col := currentCol
  accumulatorModule.io.dataIn := vecDotProductModule.io.result
  accumulatorModule.io.writeEnable := false.B
  accumulatorModule.io.clear := false.B
  io.finish := false.B
  io.dataOut := memoryModule.io.dataOut.asSInt

  /* FSM */
  switch(state) {
    is(ready) {
      /* Wait for start or store signal */
      when(io.start) {
        state := load
      }
      when(io.store) {
        state := store
      }
    }
    is(load) {
      memoryModule.io.dataSize := cfg.byteOp.U
      /* Load row */
      when(loadState === loadRow) {
        memoryModule.io.address := io.baseAddressA + currentRow * io.rowAddrDiffA + index

        when(waitMem) {
          waitMem := false.B
        } otherwise {
          waitMem := true.B
          bufferRow(index) := memoryModule.io.dataOut
          if (cfg.debugEnable) {
            printf(
              "Load row: %d, index: %d under baseAddress: %d, real address: %d, data: %d\n",
              currentRow,
              index,
              io.baseAddressA,
              io.baseAddressA + currentRow * io.rowAddrDiffA + index,
              memoryModule.io.dataOut
            )
          }
          when(index === (cfg.matSize - 1).U) {
            index := 0.U
            loadState := loadCol
          } otherwise {
            index := index + 1.U
          }
        }
      }
      /* Load column */
      when(loadState === loadCol) {
        memoryModule.io.address := io.baseAddressB + index * io.rowAddrDiffB + currentCol

        when(waitMem) {
          waitMem := false.B
        } otherwise {
          waitMem := true.B
          bufferCol(index) := memoryModule.io.dataOut
          if (cfg.debugEnable) {
            printf(
              "Load col: %d, index: %d under baseAddress: %d, real address: %d, data: %d\n",
              currentCol,
              index,
              io.baseAddressB,
              io.baseAddressB + index * io.rowAddrDiffB + currentCol,
              memoryModule.io.dataOut
            )
          }
          when(index === (cfg.matSize - 1).U) {
            index := 0.U
            state := busy
          } otherwise {
            index := index + 1.U
          }
        }
      }
    }
    is(busy) {
      /* Send result to accumulator */
      accumulatorModule.io.writeEnable := true.B
      if (cfg.debugEnable) {
        printf(
          "Write result to accumulator at row: %d, col: %d, data: %d\n",
          currentRow,
          currentCol,
          vecDotProductModule.io.result
        )
      }

      /* Update matrix loop */
      when(currentCol === (cfg.matSize - 1).U) {
        loadState := loadRow
        currentCol := 0.U
        when(currentRow === (cfg.matSize - 1).U) {
          currentRow := 0.U
          state := finish
        } otherwise {
          currentRow := currentRow + 1.U
          state := load
        }
      } otherwise {
        loadState := loadCol
        currentCol := currentCol + 1.U
        state := load
      }
    }
    is(finish) {
      /* Send finish signal */
      io.finish := true.B
      state := ready
      when(io.store) {
        state := store
      }
    }
    is(store) {
      /* Store result to memory */
      memoryModule.io.dataIn := accumulatorModule.io.dataOut.asUInt
      memoryModule.io.address := io.baseAddressC + currentRow * io.rowAddrDiffC + currentCol * 4.U
      memoryModule.io.writeEnable := true.B
      memoryModule.io.dataSize := cfg.wordOp.U
      if (cfg.debugEnable) {
        printf(
          "Store result to memory at row: %d, col: %d under baseAddress: %d, real address: %d, data: %d\n",
          currentRow,
          currentCol,
          io.baseAddressC,
          io.baseAddressC + currentRow * io.rowAddrDiffC + currentCol * 4.U,
          accumulatorModule.io.dataOut
        )
      }

      /* Update matrix loop */
      when(currentCol === (cfg.matSize - 1).U) {
        currentCol := 0.U
        when(currentRow === (cfg.matSize - 1).U) {
          currentRow := 0.U
          state := finish
          accumulatorModule.io.clear := true.B
        } otherwise {
          currentRow := currentRow + 1.U
        }
      } otherwise {
        currentCol := currentCol + 1.U
      }
    }
  }
}
