# Enum-Transpiler
Wanted this:

```go
type FieldName enum(int32) {
  KeyWord,
  Numeric,
  Identifier,
  Unknown,
  EndOfFile
}

type FieldName enum {
  KeyWord,
  Numeric
}

```

Failed to create a go-module, maybe later.
