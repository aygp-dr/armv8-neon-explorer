;;; neon-setup.el --- Configuration for ARM NEON development

;; Add syntax highlighting for ARM NEON intrinsics
(font-lock-add-keywords
 'c-mode
 '(("\\<v[a-z]+[q]?_[sfup][0-9]+\\>" . font-lock-builtin-face)  ;; NEON intrinsics
   ("\\<[ui]nt[0-9]+x[0-9]+_t\\>" . font-lock-type-face)      ;; NEON vector types
   ("\\<float[0-9]+x[0-9]+_t\\>" . font-lock-type-face)))     ;; NEON float vector types

;; Define function to insert ARM NEON vector operation skeleton
(defun insert-neon-vector-operation ()
  (interactive)
  (let ((type (completing-read "Vector type: " 
                              '("int8x16_t" "uint8x16_t" 
                                "int16x8_t" "uint16x8_t"
                                "int32x4_t" "uint32x4_t"
                                "float32x4_t" "float64x2_t")))
        (operation (completing-read "Operation: "
                                   '("add" "sub" "mul" "div" "max" "min"))))
    (let ((op-prefix (cond
                      ((string-match "float" type) "v")
                      (t "v")))
          (op-suffix (cond
                      ((string-match "float32" type) "q_f32")
                      ((string-match "float64" type) "q_f64")
                      ((string-match "int8" type) "q_s8")
                      ((string-match "uint8" type) "q_u8")
                      ((string-match "int16" type) "q_s16")
                      ((string-match "uint16" type) "q_u16")
                      ((string-match "int32" type) "q_s32")
                      ((string-match "uint32" type) "q_u32")
                      (t ""))))
      (insert (format "
/* NEON %s operation */
void vector_%s_%s(const %s *a, const %s *b, %s *result, size_t count) {
    for (size_t i = 0; i < count; i += %d) {
        %s va = vld1%s(a + i);
        %s vb = vld1%s(b + i);
        %s vc = %s%s%s(va, vb);
        vst1%s(result + i, vc);
    }
}
" 
                     operation
                     operation
                     op-suffix
                     type type type
                     (cond
                      ((string-match "x16" type) 16)
                      ((string-match "x8" type) 8)
                      ((string-match "x4" type) 4)
                      ((string-match "x2" type) 2)
                      (t 1))
                     type op-suffix
                     type op-suffix
                     type
                     op-prefix operation op-suffix
                     op-suffix)))))

;; Add org-mode NEON code block templates
(with-eval-after-load 'org
  (add-to-list 'org-structure-template-alist
               '("neon" . "#+BEGIN_SRC c :includes <arm_neon.h>\n\n#+END_SRC")))

;; Define helper for tangle-on-save
(defun org-babel-tangle-this-file ()
  (interactive)
  (org-babel-tangle))

;; Keybindings
(global-set-key (kbd "C-c n f") 'insert-neon-vector-operation)
(with-eval-after-load 'org
  (define-key org-mode-map (kbd "C-c C-v t") 'org-babel-tangle-this-file))

(provide 'neon-setup)
