; ---------------------------------------------------------------------------
; interrupt.s
; ---------------------------------------------------------------------------
;
; Interrupt handler.
;
; Checks for a BRK instruction and returns from all valid interrupts.

.include "../gen/modules_enabled.inc"
.include "zeropage.inc"

.import   _frameflag
.import   _draw_busy
.import   _tick_music, _auto_tick_music
.export   _irq_int, _nmi_int

.ifdef ENABLE_MODULE_DRAWQUEUE
.import _next_draw_queue, _queue_end, _queue_pending, _queue_start
.endif

.import         popax, __ZP_START__, jmpvec

.pc02

DMA_Start = $4006

.bss

irqsp:  .res    2

zpsave: .res    zpsavespace

music_stack_area: .res 256
music_stack_area_end: .res 1

.segment  "CODE"

.PC02                             ; Force 65C02 assembly mode

call_tick_music:
        ; Save our zero page locations
@L1:    ldx     #.sizeof(::zpsave)-1
@L2:    lda     <__ZP_START__,x
        sta     zpsave,x
        dex
        bpl     @L2

        ; Save jmpvec
        lda     jmpvec+1
        pha
        lda     jmpvec+2
        pha

        ; Set C level interrupt stack
        lda     #<music_stack_area_end
        ldx     #>music_stack_area_end
        sta     sp
        stx     sp+1

        ; Call C level interrupt request handler
        jsr     _tick_music

        ; Mark interrupt handled / not handled
        lsr

        ; Restore our zero page content
        ldx     #.sizeof(::zpsave)-1
@L3:    lda     zpsave,x
        sta     <__ZP_START__,x
        dex
        bpl     @L3

        ; Restore jmpvec and return
        pla
        sta     jmpvec+2
        pla
        sta     jmpvec+1
        rts    

; ---------------------------------------------------------------------------
; Non-maskable interrupt (NMI) service routine

_nmi_int:
        PHA
        PHX
        PHY
        LDA $1FFF
        BNE nmi_done
        STZ _frameflag
        LDA _auto_tick_music
        BEQ nmi_after_tick_music
        JSR call_tick_music
nmi_after_tick_music:
nmi_done:
        PLY
        PLX
        PLA
        RTI

        

; ---------------------------------------------------------------------------
; Maskable interrupt (IRQ) service routine

.ifdef ENABLE_MODULE_DRAWQUEUE
_irq_int:
        PHX                    ; Save X register contents to stack
        PHA
        PHY

        ;make sure DMA isn't running then compare head and tail of queue
        ;to determine whether there is more to process
        STZ DMA_Start
        STZ _queue_pending
        STZ _draw_busy
        LDA _queue_start
        CMP _queue_end
        BEQ finish_irq
        JSR _next_draw_queue
finish_irq:
        PLY
        PLA                    ; Restore accumulator contents
        PLX                    ; Restore X register contents
        RTI                    ; Return from all IRQ interrupts
.else
_irq_int:
        STZ DMA_Start
        STZ _draw_busy
        RTI
.endif