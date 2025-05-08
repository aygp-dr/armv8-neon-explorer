;;; neon-project.el --- Project-specific configuration for ARM NEON development

;; Add syntax highlighting for ARM NEON intrinsics
(defun setup-neon-syntax-highlighting ()
  (font-lock-add-keywords
   nil
   '(("\\<v[a-z]+[q]?_[sfup][0-9]+\\>" . font-lock-builtin-face)  ;; NEON intrinsics
     ("\\<[ui]nt[0-9]+x[0-9]+_t\\>" . font-lock-type-face)        ;; NEON vector types
     ("\\<float[0-9]+x[0-9]+_t\\>" . font-lock-type-face))))      ;; NEON float vector types

;; Add hook for C mode
(add-hook 'c-mode-hook 'setup-neon-syntax-highlighting)

;; Custom NEON code snippets
(defvar neon-snippets
  '(("vld" . "// Load 4 elements into a vector register\nfloat32x4_t ${1:vec} = vld1q_f32(${2:ptr});\n$0")
    ("vst" . "// Store 4 elements from a vector register\nvst1q_f32(${1:ptr}, ${2:vec});\n$0")
    ("vadd" . "// Add two vector registers\nfloat32x4_t ${1:result} = vaddq_f32(${2:a}, ${3:b});\n$0")
    ("vsub" . "// Subtract two vector registers\nfloat32x4_t ${1:result} = vsubq_f32(${2:a}, ${3:b});\n$0")
    ("vmul" . "// Multiply two vector registers\nfloat32x4_t ${1:result} = vmulq_f32(${2:a}, ${3:b});\n$0")
    ("vfma" . "// Fused multiply-add: d = d + (a * b)\nfloat32x4_t ${1:d} = vfmaq_f32(${1:d}, ${2:a}, ${3:b});\n$0")))

;; Add snippets to yasnippet
(defun neon-add-snippets ()
  (interactive)
  (require 'yasnippet)
  (let ((snip-dir (expand-file-name "snippets/c-mode" user-emacs-directory)))
    (unless (file-exists-p snip-dir)
      (make-directory snip-dir t))
    
    (dolist (snippet neon-snippets)
      (let ((name (car snippet))
            (content (cdr snippet)))
        (with-temp-file (expand-file-name name snip-dir)
          (insert "# -*- mode: snippet -*-\n")
          (insert (format "# name: %s - NEON\n" name))
          (insert (format "# key: %s\n" name))
          (insert "# --\n")
          (insert content))))))

;; Function to insert ARM NEON vector function skeleton
(defun insert-neon-function ()
  (interactive)
  (let* ((types '("int8x16_t" "uint8x16_t" "int16x8_t" "uint16x8_t" 
                 "int32x4_t" "uint32x4_t" "float32x4_t" "float64x2_t"))
         (operations '("add" "sub" "mul" "div" "max" "min" "abs" "neg"))
         (type (completing-read "Vector type: " types))
         (operation (completing-read "Operation: " operations))
         (func-name (format "neon_%s_%s" 
                           operation
                           (cond
                            ((string-match "float32" type) "f32")
                            ((string-match "float64" type) "f64")
                            ((string-match "int8" type) "s8")
                            ((string-match "uint8" type) "u8")
                            ((string-match "int16" type) "s16")
                            ((string-match "uint16" type) "u16")
                            ((string-match "int32" type) "s32")
                            ((string-match "uint32" type) "u32")
                            (t "")))))
    (insert (format "/**
 * %s 
 * Performs %s operation on vectors of type %s
 */
void %s(const %s* a, const %s* b, %s* result, size_t count) {
    // Process elements in chunks of vector size
    size_t vector_size = %d;
    size_t vector_count = count / vector_size;
    
    for (size_t i = 0; i < vector_count; i++) {
        // Load vectors
        %s va = vld1q_%s(a + i * vector_size);
        %s vb = vld1q_%s(b + i * vector_size);
        
        // Perform operation
        %s vc = v%sq_%s(va, vb);
        
        // Store result
        vst1q_%s(result + i * vector_size, vc);
    }
    
    // Handle remaining elements
    for (size_t i = vector_count * vector_size; i < count; i++) {
        result[i] = a[i] %s b[i];
    }
}
"
            func-name
            operation
            type
            func-name
            type type type
            (cond
             ((string-match "x16" type) 16)
             ((string-match "x8" type) 8)
             ((string-match "x4" type) 4)
             ((string-match "x2" type) 2)
             (t 1))
            type
            (cond
             ((string-match "float32" type) "f32")
             ((string-match "float64" type) "f64")
             ((string-match "int8" type) "s8")
             ((string-match "uint8" type) "u8")
             ((string-match "int16" type) "s16")
             ((string-match "uint16" type) "u16")
             ((string-match "int32" type) "s32")
             ((string-match "uint32" type) "u32")
             (t ""))
            type
            (cond
             ((string-match "float32" type) "f32")
             ((string-match "float64" type) "f64")
             ((string-match "int8" type) "s8")
             ((string-match "uint8" type) "u8")
             ((string-match "int16" type) "s16")
             ((string-match "uint16" type) "u16")
             ((string-match "int32" type) "s32")
             ((string-match "uint32" type) "u32")
             (t ""))
            type
            operation
            (cond
             ((string-match "float32" type) "f32")
             ((string-match "float64" type) "f64")
             ((string-match "int8" type) "s8")
             ((string-match "uint8" type) "u8")
             ((string-match "int16" type) "s16")
             ((string-match "uint16" type) "u16")
             ((string-match "int32" type) "s32")
             ((string-match "uint32" type) "u32")
             (t ""))
            (cond
             ((string-match "float32" type) "f32")
             ((string-match "float64" type) "f64")
             ((string-match "int8" type) "s8")
             ((string-match "uint8" type) "u8")
             ((string-match "int16" type) "s16")
             ((string-match "uint16" type) "u16")
             ((string-match "int32" type) "s32")
             ((string-match "uint32" type) "u32")
             (t ""))
            (cond
             ((string= operation "add") "+")
             ((string= operation "sub") "-")
             ((string= operation "mul") "*")
             ((string= operation "div") "/")
             ((string= operation "max") "> b[i] ? a[i] : b[i]")
             ((string= operation "min") "< b[i] ? a[i] : b[i]")
             ((string= operation "abs") "< 0 ? -a[i] : a[i]")
             ((string= operation "neg") "* -1")
             (t "+"))
            ))))

;; Keybindings
(defun setup-neon-keybindings ()
  (local-set-key (kbd "C-c n f") 'insert-neon-function)
  (local-set-key (kbd "C-c n s") 'yas-insert-snippet))

(add-hook 'c-mode-hook 'setup-neon-keybindings)

;; Helper for org-mode NEON examples
(with-eval-after-load 'org
  (add-to-list 'org-structure-template-alist
               '("neon" . "#+BEGIN_SRC c :includes <arm_neon.h>\n\n#+END_SRC")))

;; Compile command setup
(defun setup-neon-compile-command ()
  (setq compile-command 
        (format "make -C %s" 
                (locate-dominating-file buffer-file-name "Makefile"))))

(add-hook 'c-mode-hook 'setup-neon-compile-command)

;; Automatically tangle org files on save
(defun org-babel-tangle-on-save ()
  (when (string= (file-name-extension buffer-file-name) "org")
    (org-babel-tangle)))

;; Enable this if you want auto-tangling (commented by default)
;; (add-hook 'after-save-hook 'org-babel-tangle-on-save)

;; Define a function to build the project in a Docker container
(defun docker-make (target)
  (interactive "sTarget: ")
  (let ((default-directory (locate-dominating-file buffer-file-name "Makefile")))
    (compile (format "./container.sh make %s" target))))

;; Define a function to get a shell in the container
(defun docker-shell ()
  (interactive)
  (let ((default-directory (locate-dominating-file buffer-file-name "Makefile")))
    (async-shell-command "./container.sh shell")))

;; Keybindings for Docker functions
(global-set-key (kbd "C-c d m") 'docker-make)
(global-set-key (kbd "C-c d s") 'docker-shell)

(provide 'neon-project)

;;; neon-project.el --- Project-specific configuration for ARM NEON development

;; Add syntax highlighting for ARM NEON intrinsics
(defun setup-neon-syntax-highlighting ()
  (font-lock-add-keywords
   nil
   '(("\\<v[a-z]+[q]?_[sfup][0-9]+\\>" . font-lock-builtin-face)  ;; NEON intrinsics
     ("\\<[ui]nt[0-9]+x[0-9]+_t\\>" . font-lock-type-face)        ;; NEON vector types
     ("\\<float[0-9]+x[0-9]+_t\\>" . font-lock-type-face))))      ;; NEON float vector types

;; Add hook for C mode
(add-hook 'c-mode-hook 'setup-neon-syntax-highlighting)

;; Custom NEON code snippets
(defvar neon-snippets
  '(("vld" . "// Load 4 elements into a vector register\nfloat32x4_t ${1:vec} = vld1q_f32(${2:ptr});\n$0")
    ("vst" . "// Store 4 elements from a vector register\nvst1q_f32(${1:ptr}, ${2:vec});\n$0")
    ("vadd" . "// Add two vector registers\nfloat32x4_t ${1:result} = vaddq_f32(${2:a}, ${3:b});\n$0")
    ("vsub" . "// Subtract two vector registers\nfloat32x4_t ${1:result} = vsubq_f32(${2:a}, ${3:b});\n$0")
    ("vmul" . "// Multiply two vector registers\nfloat32x4_t ${1:result} = vmulq_f32(${2:a}, ${3:b});\n$0")
    ("vfma" . "// Fused multiply-add: d = d + (a * b)\nfloat32x4_t ${1:d} = vfmaq_f32(${1:d}, ${2:a}, ${3:b});\n$0")))

;; Add snippets to yasnippet
(defun neon-add-snippets ()
  (interactive)
  (require 'yasnippet)
  (let ((snip-dir (expand-file-name "snippets/c-mode" user-emacs-directory)))
    (unless (file-exists-p snip-dir)
      (make-directory snip-dir t))
    
    (dolist (snippet neon-snippets)
      (let ((name (car snippet))
            (content (cdr snippet)))
        (with-temp-file (expand-file-name name snip-dir)
          (insert "# -*- mode: snippet -*-\n")
          (insert (format "# name: %s - NEON\n" name))
          (insert (format "# key: %s\n" name))
          (insert "# --\n")
          (insert content))))))

;; Function to insert ARM NEON vector function skeleton
(defun insert-neon-function ()
  (interactive)
  (let* ((types '("int8x16_t" "uint8x16_t" "int16x8_t" "uint16x8_t" 
                 "int32x4_t" "uint32x4_t" "float32x4_t" "float64x2_t"))
         (operations '("add" "sub" "mul" "div" "max" "min" "abs" "neg"))
         (type (completing-read "Vector type: " types))
         (operation (completing-read "Operation: " operations))
         (func-name (format "neon_%s_%s" 
                           operation
                           (cond
                            ((string-match "float32" type) "f32")
                            ((string-match "float64" type) "f64")
                            ((string-match "int8" type) "s8")
                            ((string-match "uint8" type) "u8")
                            ((string-match "int16" type) "s16")
                            ((string-match "uint16" type) "u16")
                            ((string-match "int32" type) "s32")
                            ((string-match "uint32" type) "u32")
                            (t "")))))
    (insert (format "/**
 * %s 
 * Performs %s operation on vectors of type %s
 */
void %s(const %s* a, const %s* b, %s* result, size_t count) {
    // Process elements in chunks of vector size
    size_t vector_size = %d;
    size_t vector_count = count / vector_size;
    
    for (size_t i = 0; i < vector_count; i++) {
        // Load vectors
        %s va = vld1q_%s(a + i * vector_size);
        %s vb = vld1q_%s(b + i * vector_size);
        
        // Perform operation
        %s vc = v%sq_%s(va, vb);
        
        // Store result
        vst1q_%s(result + i * vector_size, vc);
    }
    
    // Handle remaining elements
    for (size_t i = vector_count * vector_size; i < count; i++) {
        result[i] = a[i] %s b[i];
    }
}
"
            func-name
            operation
            type
            func-name
            type type type
            (cond
             ((string-match "x16" type) 16)
             ((string-match "x8" type) 8)
             ((string-match "x4" type) 4)
             ((string-match "x2" type) 2)
             (t 1))
            type
            (cond
             ((string-match "float32" type) "f32")
             ((string-match "float64" type) "f64")
             ((string-match "int8" type) "s8")
             ((string-match "uint8" type) "u8")
             ((string-match "int16" type) "s16")
             ((string-match "uint16" type) "u16")
             ((string-match "int32" type) "s32")
             ((string-match "uint32" type) "u32")
             (t ""))
            type
            (cond
             ((string-match "float32" type) "f32")
             ((string-match "float64" type) "f64")
             ((string-match "int8" type) "s8")
             ((string-match "uint8" type) "u8")
             ((string-match "int16" type) "s16")
             ((string-match "uint16" type) "u16")
             ((string-match "int32" type) "s32")
             ((string-match "uint32" type) "u32")
             (t ""))
            type
            operation
            (cond
             ((string-match "float32" type) "f32")
             ((string-match "float64" type) "f64")
             ((string-match "int8" type) "s8")
             ((string-match "uint8" type) "u8")
             ((string-match "int16" type) "s16")
             ((string-match "uint16" type) "u16")
             ((string-match "int32" type) "s32")
             ((string-match "uint32" type) "u32")
             (t ""))
            (cond
             ((string-match "float32" type) "f32")
             ((string-match "float64" type) "f64")
             ((string-match "int8" type) "s8")
             ((string-match "uint8" type) "u8")
             ((string-match "int16" type) "s16")
             ((string-match "uint16" type) "u16")
             ((string-match "int32" type) "s32")
             ((string-match "uint32" type) "u32")
             (t ""))
            (cond
             ((string= operation "add") "+")
             ((string= operation "sub") "-")
             ((string= operation "mul") "*")
             ((string= operation "div") "/")
             ((string= operation "max") "> b[i] ? a[i] : b[i]")
             ((string= operation "min") "< b[i] ? a[i] : b[i]")
             ((string= operation "abs") "< 0 ? -a[i] : a[i]")
             ((string= operation "neg") "* -1")
             (t "+"))
            ))))

;; Keybindings
(defun setup-neon-keybindings ()
  (local-set-key (kbd "C-c n f") 'insert-neon-function)
  (local-set-key (kbd "C-c n s") 'yas-insert-snippet))

(add-hook 'c-mode-hook 'setup-neon-keybindings)

;; Helper for org-mode NEON examples
(with-eval-after-load 'org
  (add-to-list 'org-structure-template-alist
               '("neon" . "#+BEGIN_SRC c :includes <arm_neon.h>\n\n#+END_SRC")))

;; Compile command setup
(defun setup-neon-compile-command ()
  (setq compile-command 
        (format "make -C %s" 
                (locate-dominating-file buffer-file-name "Makefile"))))

(add-hook 'c-mode-hook 'setup-neon-compile-command)

;; Automatically tangle org files on save
(defun org-babel-tangle-on-save ()
  (when (string= (file-name-extension buffer-file-name) "org")
    (org-babel-tangle)))

;; Enable this if you want auto-tangling (commented by default)
;; (add-hook 'after-save-hook 'org-babel-tangle-on-save)

;; Define a function to build the project in a Docker container
(defun docker-make (target)
  (interactive "sTarget: ")
  (let ((default-directory (locate-dominating-file buffer-file-name "Makefile")))
    (compile (format "./container.sh make %s" target))))

;; Define a function to get a shell in the container
(defun docker-shell ()
  (interactive)
  (let ((default-directory (locate-dominating-file buffer-file-name "Makefile")))
    (async-shell-command "./container.sh shell")))

;; Keybindings for Docker functions
(global-set-key (kbd "C-c d m") 'docker-make)
(global-set-key (kbd "C-c d s") 'docker-shell)

(provide 'neon-project)
