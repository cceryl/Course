package GEMM

import chisel3._
import chisel3.util._

case class MemoryConfigs(
    /* Fixed */
    dataWidth: Int = 32,
    /* Configurable */
    addrWidth: Int = 32,
    memSize: Int = 0x1000,
    /* Enum */
    byteOp: Int = 0,
    halfOp: Int = 1,
    wordOp: Int = 2
)

class MemoryModule(cfg: MemoryConfigs) extends Module {

  /* Read during write behavior is undefined */
  /* The output is updated on the next cycle */

  val io = IO(new Bundle {
    val dataIn = Input(UInt(cfg.dataWidth.W))
    val address = Input(UInt(cfg.addrWidth.W))
    val dataSize = Input(UInt(2.W))
    val writeEnable = Input(Bool())

    val dataOut = Output(UInt(cfg.dataWidth.W))
  })

  assert(cfg.dataWidth == 32, "Only 32-bit data width is supported")
  assert(
    io.dataSize === cfg.byteOp.U || io.dataSize === cfg.halfOp.U || io.dataSize === cfg.wordOp.U,
    "Invalid data size"
  )

  val mem = SyncReadMem(cfg.memSize, UInt(8.W))

  /* Write operation */
  when(io.writeEnable) {
    switch(io.dataSize) {
      is(cfg.byteOp.U) {
        mem.write(io.address, io.dataIn(7, 0))
      }
      is(cfg.halfOp.U) {
        mem.write(io.address, io.dataIn(7, 0))
        mem.write(io.address + 1.U, io.dataIn(15, 8))
      }
      is(cfg.wordOp.U) {
        mem.write(io.address, io.dataIn(7, 0))
        mem.write(io.address + 1.U, io.dataIn(15, 8))
        mem.write(io.address + 2.U, io.dataIn(23, 16))
        mem.write(io.address + 3.U, io.dataIn(31, 24))
      }
    }
  }

  /* Read operation */
  io.dataOut := 0.U
  switch(io.dataSize) {
    is(cfg.byteOp.U) {
      io.dataOut := Cat(Fill(24, 0.U), mem.read(io.address))
    }
    is(cfg.halfOp.U) {
      io.dataOut := Cat(
        Fill(16, 0.U),
        mem.read(io.address + 1.U),
        mem.read(io.address)
      )
    }
    is(cfg.wordOp.U) {
      io.dataOut := Cat(
        mem.read(io.address + 3.U),
        mem.read(io.address + 2.U),
        mem.read(io.address + 1.U),
        mem.read(io.address)
      )
    }
  }
}
