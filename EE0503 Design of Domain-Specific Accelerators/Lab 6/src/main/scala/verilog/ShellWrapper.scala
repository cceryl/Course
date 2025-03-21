package Lab6_FPGA

import chisel3._
import chisel3.util._

import Lab6._

class FPGAShellWrapper(configs: MatrixMultiplicationTopConfigs)
    extends RawModule {
  // IO
  val start = IO(Input(Bool()))
  val check = IO(Input(Bool()))
  val clock = IO(Input(Clock()))
  val reset = IO(Input(Bool()))

  val finish = IO(Output(Bool()))
  val correct = IO(Output(Bool()))

  // Logic
  val shell = withClockAndReset(clock, !reset) {
    Module(new FPGAShell(configs))
  }

  shell.io.start := !start
  shell.io.check := !check
  finish := shell.io.finish
  correct := shell.io.correct
}
