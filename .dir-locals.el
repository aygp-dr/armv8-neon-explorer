;;; Directory Local Variables
;;; For more information see (info "(emacs) Directory Variables")

((nil . ((fill-column . 80)
         (indent-tabs-mode . nil)
         (tab-width . 4)
         (require-final-newline . t)))
 (org-mode . ((org-confirm-babel-evaluate . nil)
              (org-src-fontify-natively . t)
              (org-babel-default-header-args:C . ((:flags . "-std=c11 -O3 -march=armv8-a+simd")))
              (org-babel-default-header-args:sh . ((:results . "output verbatim replace")))
              (eval . (progn
                       (require 'neon-setup)
                       (require 'ox-md)
                       (require 'ob-C)
                       (require 'ob-shell)
                       (require 'ob-dot)))
              (org-babel-tangle-lang-exts . (("C" . "c") ("cpp" . "cpp") ("shell" . "sh")))))
 (c-mode . ((c-basic-offset . 4)
            (c-default-style . "linux")
            (flycheck-gcc-language-standard . "c11")
            (flycheck-clang-language-standard . "c11")
            (compile-command . "make -C ${project-root} all")))
 (c++-mode . ((c-basic-offset . 4)
              (c-default-style . "linux")
              (flycheck-gcc-language-standard . "c++17")
              (flycheck-clang-language-standard . "c++17"))))
