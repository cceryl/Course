package Lab3

import chisel3._
import chisel3.util._

case class MatrixMultiplicationConfigs(
    /* Fixed */
    matDWidth: Int = 8,
    resDWidth: Int = 32,
    /* Configurable */
    matSize: Int = 16,
    memSize: Int = 0x1000,
    addrWidth: Int = 32,
    baseAddressA: Int = 0x0000,
    baseAddressB: Int = 0x0200,
    baseAddressC: Int = 0x0400,
    isSignedA: Boolean = false,
    isSignedB: Boolean = false,
    /* Enum */
    selectA: Int = 0,
    selectB: Int = 1,
    byteOp: Int = 0,
    halfOp: Int = 1,
    wordOp: Int = 2
)

class MatrixMultiplicationModule(cfg: MatrixMultiplicationConfigs)
    extends Module {

  assert(cfg.matDWidth == 8, "Only support 8-bit data input width")
  assert(cfg.resDWidth == 32, "Only support 32-bit data output width")

  /* Define I/O */
  val io = IO(new Bundle {
    val start = Input(Bool())
    val dataIn = Input(UInt(cfg.matDWidth.W))
    val address = Input(UInt(cfg.addrWidth.W))
    val writeEnable = Input(Bool())

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
    new VecDotProductModule(
      VecDotProductConfigs(
        vecDWidth = cfg.matDWidth,
        resDWidth = cfg.resDWidth,
        vecLen = cfg.matSize,
        isSignedA = cfg.isSignedA,
        isSignedB = cfg.isSignedB
      )
    )
  )

  /* Define FSM status*/
  val ready :: load :: busy :: finish :: Nil = Enum(4)
  val state = RegInit(ready)
  val currentRow = RegInit(0.U(log2Ceil(cfg.matSize).W))
  val currentCol = RegInit(0.U(log2Ceil(cfg.matSize).W))
  val loadRow :: loadCol :: Nil = Enum(2)
  val loadState = RegInit(loadRow)
  val index = RegInit(0.U(log2Ceil(cfg.matSize).W))
  val waitMem = RegInit(false.B)

  /* Default input & output */
  memoryModule.io.dataIn := io.dataIn
  memoryModule.io.address := io.address
  memoryModule.io.dataSize := cfg.wordOp.U
  memoryModule.io.writeEnable := io.writeEnable
  vecDotProductModule.io.vecA := bufferRow
  vecDotProductModule.io.vecB := bufferCol
  io.finish := false.B
  io.dataOut := memoryModule.io.dataOut.asSInt

  /* FSM */
  switch(state) {
    is(ready) {
      /* Wait for start signal */
      when(io.start) {
        state := load
      }
    }
    is(load) {
      memoryModule.io.dataSize := cfg.byteOp.U
      /* Load row */
      when(loadState === loadRow) {
        memoryModule.io.address := cfg.baseAddressA.U + currentRow * cfg.matSize.U + index
        when(waitMem) {
          waitMem := false.B
        } otherwise {
          waitMem := true.B
          bufferRow(index) := memoryModule.io.dataOut
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
        memoryModule.io.address := cfg.baseAddressB.U + index * cfg.matSize.U + currentCol
        when(waitMem) {
          waitMem := false.B
        } otherwise {
          waitMem := true.B
          bufferCol(index) := memoryModule.io.dataOut
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
      /* Write result to memoryModule */
      memoryModule.io.dataIn := vecDotProductModule.io.result.asUInt
      memoryModule.io.address := cfg.baseAddressC.U + (currentRow * cfg.matSize.U + currentCol) * 4.U
      memoryModule.io.dataSize := cfg.wordOp.U
      memoryModule.io.writeEnable := true.B

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
    }
  }
}
