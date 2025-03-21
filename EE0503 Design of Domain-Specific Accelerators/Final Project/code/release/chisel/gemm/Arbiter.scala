package ROCC_Lab

import chisel3._ // VecInit

import chisel3.util._ // MuxCase
import freechips.rocketchip.tile._
import freechips.rocketchip.subsystem._ // for getting subsystembus parameter
import org.chipsalliance.cde.config._
import freechips.rocketchip.diplomacy._ // LazyModule
import freechips.rocketchip.rocket._ // cache 接口
import freechips.rocketchip.util._

import ROCC_Lab_config._

// requeset the BankArray read or write
class ReqPackage(cfg: MatrixMultiplicationConfigs) extends Bundle {
  val addr = UInt(cfg.addrWidth.W)
  val optype = Bool()
  val dataWrittenToOnChipMem = SInt(cfg.resDWidth.W)
}

class AccessOnChipMem_Arb(cfg: MatrixMultiplicationConfigs) extends Module {
  val client_num = 2
  val io = IO(new Bundle {
    val request = Vec(client_num, Flipped(Decoupled(new ReqPackage(cfg))))
    val dataReadFromOnChipMem = Output(SInt(cfg.resDWidth.W))

    val enable = Output(Bool())
    val writeEn = Output(Bool())
    val addr = Output(UInt(cfg.addrWidth.W))
    val dataOut = Output(SInt(cfg.resDWidth.W))
    val dataIn = Input(SInt(cfg.resDWidth.W))

  })

  // init
  io.enable := false.B
  io.writeEn := false.B
  io.addr := 0.U
  io.dataOut := 0.S

  io.dataReadFromOnChipMem := io.dataIn

  val arbiter = Module(new RRArbiter(new ReqPackage(cfg), 2))

  for (i <- 0 until client_num) {
    arbiter.io.in(i) <> io.request(i)
  }

  // always ready for receiving a request
  arbiter.io.out.ready := true.B

  // 表明接收到了一个请求
  when(arbiter.io.out.fire)
  {
    val optype = arbiter.io.out.bits.optype

    // Write instantly
    when(optype === cfg.onChipWrite)
    {
      io.enable := true.B
      io.writeEn := true.B
      io.addr := arbiter.io.out.bits.addr
      io.dataOut := arbiter.io.out.bits.dataWrittenToOnChipMem
    }.otherwise // Read
    {
      io.enable := true.B
      io.writeEn := false.B
      io.addr := arbiter.io.out.bits.addr
    }
  }
}
