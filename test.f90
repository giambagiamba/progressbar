module pbar_type
       use, intrinsic :: iso_c_binding, only : c_char, c_long, c_double
        type, bind(C) :: pbar_t
                character(c_char) :: bar
                integer(c_long) :: len, Num, max, perc, nblks, err, file
                real(c_double) :: start
                integer(c_long) :: NT
        end type pbar_t
end module pbar_type

program main
        use pbar_type
type(pbar_t) :: barra

        INTERFACE
                SUBROUTINE p_init(progbar, filename, N, el, NT) BIND(C, NAME='p_init')
                        use pbar_type
                        IMPLICIT NONE
                        type(pbar_t) :: progbar
                        character :: filename
                        integer(c_long) :: N, el, NT
                END SUBROUTINE
        END INTERFACE
        
        INTERFACE
                SUBROUTINE p_draw(progbar, argi) BIND(C, NAME='p_draw')
                        use pbar_type
                        IMPLICIT NONE
                        type(pbar_t) :: progbar
                        integer(c_long) :: argi
                END SUBROUTINE
        END INTERFACE
        
        INTERFACE
                SUBROUTINE pbar_close(progbar) BIND(C, NAME='pbar_close')
                        use pbar_type
                        IMPLICIT NONE
                        type(pbar_t) :: progbar
                END SUBROUTINE
        END INTERFACE
        
        integer(8) :: ii, jj, NN=1000000, el=20, NTH=1
        character (len=20) :: nome
        nome='reso.log'
        
        call p_init(barra, nome, NN, el, NTH)
        do ii = 1,NN
                call p_draw(barra, ii)
                do jj =1,100000
                enddo
        enddo
        call pbar_close(barra)
end program main
