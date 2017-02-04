((nil . ((eval . (let ((path (let ((d (dir-locals-find-file ".")))
                               (if (stringp d) d (car d)))))
                   (setq-local flycheck-clang-include-path
                               (list path
                                     "/usr/local/include/SDL2")))))))
