package Adder

import chisel3._
import chisel3.util._

class Uint64_Adder extends Module {

  val io = IO(new Bundle {
    val in_a = Input(UInt(64.W))
    val in_b = Input(UInt(64.W))
    val out_sum = Output(UInt(64.W))
    val out_carry = Output(Bool())
  })

  io.out_sum := io.in_a + io.in_b
  io.out_carry := io.out_sum < io.in_a
}

class Uint32_Adder extends Module {

  val io = IO(new Bundle {
    val in_a = Input(UInt(32.W))
    val in_b = Input(UInt(32.W))
    val out_sum = Output(UInt(32.W))
    val out_carry = Output(Bool())
  })

  io.out_sum := io.in_a + io.in_b
  io.out_carry := io.out_sum < io.in_a
}
