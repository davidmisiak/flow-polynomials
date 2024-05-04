# Multipole Flow Polynomials

## Dependencies

C++ utility that computes the flow polynomial coefficients:

- `make`
- `gcc`
- `Boost`

Python scripts:

- [NetworkX](https://networkx.org/)
- [Matplotlib](https://matplotlib.org/)

Analysis notebook:

- [Jupyter](https://jupyter.org/)
- [SageMath](https://www.sagemath.org/)

## Usage Examples

```bash
❯ make test
```

```bash
❯ make plot < graphs/easy/4-square
```

```bash
❯ make run output=fp < graphs/easy/4-square
[
   1 * [ -4 -3 ][ -2 -1 ]
   1 * [ -4 -1 ][ -3 -2 ]
]
```
