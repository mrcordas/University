.data
   numero: .word 20

.text

  lw $s0, numero
  #sra $a0, $t0, 1
  # add $a0, $t0, $zero
  srl $s0, $s0, 1
  add $a0, $s0, $zero
  li $v0, 1

  syscall 