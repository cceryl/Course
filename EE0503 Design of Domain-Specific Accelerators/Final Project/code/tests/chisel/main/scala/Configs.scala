package GEMM

import chisel3._
import chisel3.util._

case class MatrixMultiplicationConfigs(
    /* Fixed */
    matDWidth: Int = 8,
    resDWidth: Int = 32,
    /* Configurable */
    calcCoreSize: Int = 8,
    isSignedA: Boolean = true,
    isSignedB: Boolean = true,
    debugEnable: Boolean = false,
    /* Enum */
    selectA: Int = 0,
    selectB: Int = 1,
    selectC: Int = 2,
    byteOp: Int = 0,
    halfOp: Int = 1,
    wordOp: Int = 2,
    loadISA: UInt = 0.U,
    storeISA: UInt = 1.U,
    infoISA: UInt = 2.U,
    computeISA: UInt = 3.U,
    onChipRead: UInt = 0.U,
    onChipWrite: UInt = 1.U
) {

  /* Memory */
  def pad(size: Int, coreSize: Int): Int = {
    (size + coreSize - 1) & ~(coreSize - 1)
  }

  val maxMatARows = 1
  val maxMatACols = 784
  val maxMatBRows = 784
  val maxMatBCols = 128

  val indexWidth = 32

  val matAElemNum =
    pad(maxMatARows, calcCoreSize) * pad(maxMatACols, calcCoreSize)
  val matBElemNum =
    pad(maxMatBRows, calcCoreSize) * pad(maxMatBCols, calcCoreSize)
  val matResElemNum =
    pad(maxMatARows, calcCoreSize) * pad(maxMatBCols, calcCoreSize)

  val memSize = matAElemNum + matBElemNum + matResElemNum * 4
  val addrWidth = log2Ceil(memSize)
  val baseAddressA = 0
  val baseAddressB = matAElemNum
  val baseAddressC = matAElemNum + matBElemNum
}
