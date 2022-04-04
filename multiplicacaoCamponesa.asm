
.data
	nome: .asciiz "Fabricio Ezequiel Marcal"
	num1: .word 24 # 1º numero inteiro para teste
	num2: .word 60 # 2º numero inteiro para teste
.text
	#li $v0, 1
	#lw $a0, num1 
	lw $t0, num1($zero) 	# Carrega num1 pra registreador $t0
	lw $t1, num2($zero)	# Carrega num2 pra registreador $t0
	add $s0, $zero, $zero # adiciona zero a $s0
	
	while: # rotulo para inicio de teste
	   beq $t0, 1, exit # se $t0 == 1 --> enquando $t0 > 1 continue
	   andi $t2, $t0, 1 # $t2 = $t0 and 1 --> operação and bit compara se o ultimo bit  do primeiro numero é 0(para par) ou 1(impar)
	   bne $t2, 1, numero1Par # se o numero for par para vai para label
	   add $s0, $s0, $t1 #$s0 = $s0 + $t1 ---> soma o 2º numero a soma atual se o primeiro numero for impar
	   
	   numero1Par: # não faz nada se o numero for par
	   
	   srl $t0, $t0, 1 # $t0 = $t0 >> 1 ---> desloca um bit para direira
	   sll $t1, $t1, 1 # $t0 = $t0 << 1 --> desloca um bit para esquerda
	   
	j while
	exit:
	add $s0, $s0, $t1 # se numero da esquerda for 1 foi ultima divisão por 2
	
	li $v0, 1
	add $a0, $s0, $zero
	syscall
	
