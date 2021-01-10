module pbar_type
        use, intrinsic :: iso_c_binding, only : c_char, c_long, c_double
        type, bind(C) :: pbar_t
                character(c_char) :: bar
                integer(c_long) :: ell, num, perc, nblks, error, filed
                real(c_double) :: start
        end type pbar_t
end module pbar_type

program main
        use pbar_type
type(pbar_t) :: barra

        INTERFACE
                SUBROUTINE pbar_init(progbar, filename, N, el) BIND(C, NAME='pbar_init')
                        use pbar_type
                        IMPLICIT NONE
                        type(pbar_t) :: progbar
                        character :: filename
                        integer :: N, el
                END SUBROUTINE
        END INTERFACE
        
        INTERFACE
                SUBROUTINE pbar_draw(progbar, argi) BIND(C, NAME='pbar_draw')
                        use pbar_type
                        IMPLICIT NONE
                        type(pbar_t) :: progbar
                        integer :: argi
                END SUBROUTINE
        END INTERFACE
        
        INTERFACE
                SUBROUTINE pbar_close(progbar) BIND(C, NAME='pbar_close')
                        use pbar_type
                        IMPLICIT NONE
                        type(pbar_t) :: progbar
                END SUBROUTINE
        END INTERFACE
        
        integer :: ii, jj
        call pbar_init(barra, 'reso.log\0', 100000, 20)

        do ii = 1,100000
                jj = ii-1
                call pbar_draw(barra, ii)
        enddo
        call pbar_close(barra)
        print *,'err=',error
end program main
