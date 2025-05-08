;;; neon-babel.el --- Org-mode Babel configuration for ARM NEON development

;; Enable Babel languages
(org-babel-do-load-languages
 'org-babel-load-languages
 '((C . t)
   (python . t)
   (shell . t)
   (dot . t)))

;; Define NEON-specific header arguments
(add-to-list 'org-babel-default-header-args:C
             '(:flags . "-std=c11 -O3 -march=armv8-a+simd"))

;; Don't ask for confirmation when executing code blocks
(setq org-confirm-babel-evaluate nil)

;; Custom function to run NEON code blocks directly
(defun org-babel-execute:neon (body params)
  "Execute a block of NEON code with org-babel."
  (let* ((flags (or (cdr (assq :flags params))
                    "-std=c11 -O3 -march=armv8-a+simd"))
         (includes (or (cdr (assq :includes params))
                       "<arm_neon.h>"))
         (full-body (format "#include <stdio.h>\n#include <stdlib.h>\n#include %s\n\nint main() {\n%s\nreturn 0;\n}\n" includes body))
         (tmp-src-file (org-babel-temp-file "neon-" ".c"))
         (tmp-bin-file (concat (file-name-sans-extension tmp-src-file) ".bin"))
         (compile-cmd (format "gcc %s -o %s %s" flags tmp-bin-file tmp-src-file)))
    
    ;; Write the source code
    (with-temp-file tmp-src-file (insert full-body))
    
    ;; Compile and run
    (org-babel-eval compile-cmd "")
    (let ((results (org-babel-eval tmp-bin-file "")))
      (org-babel-reassemble-table
       results
       (org-babel-pick-name (cdr (assq :colname-names params))
                           (cdr (assq :colnames params)))
       (org-babel-pick-name (cdr (assq :rowname-names params))
                           (cdr (assq :rownames params)))))))

;; Add custom templates
(add-to-list 'org-structure-template-alist
             '("neon" . "#+BEGIN_SRC neon\n\n#+END_SRC"))

;; Add syntax highlighting for NEON code blocks
(add-to-list 'org-src-lang-modes '("neon" . c))

;; Auto-tangle file-local setup
(defun enable-org-babel-auto-tangle ()
  "Enable automatic tangling on save for the current buffer."
  (interactive)
  (add-hook 'after-save-hook #'org-babel-tangle nil t))

;; Add file variables in header for auto-tangle
(defun insert-org-tangle-setup ()
  "Insert local variables for automatic tangling."
  (interactive)
  (goto-char (point-min))
  (insert "# -*- eval: (enable-org-babel-auto-tangle) -*-\n\n"))

;; Function to create a new NEON example
(defun create-neon-example (name)
  "Create a new org-mode NEON example file."
  (interactive "sExample name: ")
  (let ((filename (concat name ".org")))
    (find-file filename)
    (erase-buffer)
    (insert (format "#+TITLE: %s - ARM NEON Example\n" (capitalize name)))
    (insert "#+AUTHOR: Aidan Pace\n")
    (insert "#+PROPERTY: header-args :eval never-export\n")
    (insert "#+OPTIONS: toc:2 num:2\n\n")
    (insert "* Introduction\n\n")
    (insert "* Implementation\n")
    (insert ":PROPERTIES:\n")
    (insert ":header-args: :tangle yes\n")
    (insert ":END:\n\n")
    (insert "#+BEGIN_SRC c\n")
    (insert (format "/**\n * %s.c\n * Example demonstrating ARM NEON SIMD implementation\n */\n" name))
    (insert "#include <stdio.h>\n")
    (insert "#include <stdlib.h>\n")
    (insert "#include <arm_neon.h>\n")
    (insert "#include \"../include/neon_utils.h\"\n\n")
    (insert "int main() {\n")
    (insert "    printf(\"ARM NEON Example: %s\\n\");\n\n" (capitalize name))
    (insert "    // Your NEON code here\n\n")
    (insert "    return 0;\n")
    (insert "}\n")
    (insert "#+END_SRC\n\n")
    (insert "* Explanation\n\n")
    (insert "* Performance Analysis\n\n")
    (insert "* Questions for Further Exploration\n\n")
    (save-buffer)))

(provide 'neon-babel)

;;; neon-babel.el --- Org-mode Babel configuration for ARM NEON development

;; Enable Babel languages
(org-babel-do-load-languages
 'org-babel-load-languages
 '((C . t)
   (python . t)
   (shell . t)
   (dot . t)))

;; Define NEON-specific header arguments
(add-to-list 'org-babel-default-header-args:C
             '(:flags . "-std=c11 -O3 -march=armv8-a+simd"))

;; Don't ask for confirmation when executing code blocks
(setq org-confirm-babel-evaluate nil)

;; Custom function to run NEON code blocks directly
(defun org-babel-execute:neon (body params)
  "Execute a block of NEON code with org-babel."
  (let* ((flags (or (cdr (assq :flags params))
                    "-std=c11 -O3 -march=armv8-a+simd"))
         (includes (or (cdr (assq :includes params))
                       "<arm_neon.h>"))
         (full-body (format "#include <stdio.h>\n#include <stdlib.h>\n#include %s\n\nint main() {\n%s\nreturn 0;\n}\n" includes body))
         (tmp-src-file (org-babel-temp-file "neon-" ".c"))
         (tmp-bin-file (concat (file-name-sans-extension tmp-src-file) ".bin"))
         (compile-cmd (format "gcc %s -o %s %s" flags tmp-bin-file tmp-src-file)))
    
    ;; Write the source code
    (with-temp-file tmp-src-file (insert full-body))
    
    ;; Compile and run
    (org-babel-eval compile-cmd "")
    (let ((results (org-babel-eval tmp-bin-file "")))
      (org-babel-reassemble-table
       results
       (org-babel-pick-name (cdr (assq :colname-names params))
                           (cdr (assq :colnames params)))
       (org-babel-pick-name (cdr (assq :rowname-names params))
                           (cdr (assq :rownames params)))))))

;; Add custom templates
(add-to-list 'org-structure-template-alist
             '("neon" . "#+BEGIN_SRC neon\n\n#+END_SRC"))

;; Add syntax highlighting for NEON code blocks
(add-to-list 'org-src-lang-modes '("neon" . c))

;; Auto-tangle file-local setup
(defun enable-org-babel-auto-tangle ()
  "Enable automatic tangling on save for the current buffer."
  (interactive)
  (add-hook 'after-save-hook #'org-babel-tangle nil t))

;; Add file variables in header for auto-tangle
(defun insert-org-tangle-setup ()
  "Insert local variables for automatic tangling."
  (interactive)
  (goto-char (point-min))
  (insert "# -*- eval: (enable-org-babel-auto-tangle) -*-\n\n"))

;; Function to create a new NEON example
(defun create-neon-example (name)
  "Create a new org-mode NEON example file."
  (interactive "sExample name: ")
  (let ((filename (concat name ".org")))
    (find-file filename)
    (erase-buffer)
    (insert (format "#+TITLE: %s - ARM NEON Example\n" (capitalize name)))
    (insert "#+AUTHOR: Aidan Pace\n")
    (insert "#+PROPERTY: header-args :eval never-export\n")
    (insert "#+OPTIONS: toc:2 num:2\n\n")
    (insert "* Introduction\n\n")
    (insert "* Implementation\n")
    (insert ":PROPERTIES:\n")
    (insert ":header-args: :tangle yes\n")
    (insert ":END:\n\n")
    (insert "#+BEGIN_SRC c\n")
    (insert (format "/**\n * %s.c\n * Example demonstrating ARM NEON SIMD implementation\n */\n" name))
    (insert "#include <stdio.h>\n")
    (insert "#include <stdlib.h>\n")
    (insert "#include <arm_neon.h>\n")
    (insert "#include \"../include/neon_utils.h\"\n\n")
    (insert "int main() {\n")
    (insert "    printf(\"ARM NEON Example: %s\\n\");\n\n" (capitalize name))
    (insert "    // Your NEON code here\n\n")
    (insert "    return 0;\n")
    (insert "}\n")
    (insert "#+END_SRC\n\n")
    (insert "* Explanation\n\n")
    (insert "* Performance Analysis\n\n")
    (insert "* Questions for Further Exploration\n\n")
    (save-buffer)))

(provide 'neon-babel)
