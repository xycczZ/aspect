<?php

/** @generate-function-entries */

namespace Xycc\Aspect
{    
    class Signature
    {
        public function __construct(private string $className, private string $methodName, private array $args) {}

        public function getArgs(): array {}

        public function getClassName(): string {}
        
        public function getMethodName(): string {}
    }

    class JoinPoint
    {
        public function __construct(private Signature $signature) {}

        public function getSignature(): Signature {}
    }

    class ProceedJoinPoint extends JoinPoint
    {
        private $result; /** mixed or void */
        private bool $isVoid = false;

        public function __construct(Signature $signature, private \Closure $origin) {}

        public function proceed(): mixed {}

        public function proceedWith(mixed ...$args): mixed {}
    }
}

/**
 * @throw RuntimeException
 */
function add_aspect(string $expr, array $advises): bool {};
