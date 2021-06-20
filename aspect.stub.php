<?php

/** @generate-function-entries */

namespace Xycc
{    
    class Signature
    {
        private string $className;
        private string $methodName;
        private array $args;

        public function getArgs(): array {};

        public function getClassName(): string {};
        
        public function getMethodName(): string {};
    }
}

function add_aspect(string $expr, array $advises): bool {};
