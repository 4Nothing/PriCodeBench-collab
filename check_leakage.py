import json, glob

for p in sorted(glob.glob("**/results/rag*/**/results.jsonl", recursive=True)):
    n = leak = 0
    for line in open(p):
        if not line.strip():
            continue
        d = json.loads(line)
        sut = d["snapshot"]["task"]["sut_function"]
        prompt = d["snapshot"].get("prompt_text", "")
        if "// Function: " + sut in prompt:
            leak += 1
        n += 1
    flag = "OK " if leak == 0 else "!! 泄漏"
    print(f"[{flag}] {leak:3d}/{n:3d}  {p}")
