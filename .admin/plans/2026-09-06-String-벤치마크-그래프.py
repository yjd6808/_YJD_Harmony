# -*- coding: utf-8 -*-
"""
String 후보(S0~S7, R0~R1) 벤치마크 그래프 스크립트
- 가로축: 후보 (R0, S0, S1, S2, S3, S4, S5, S6, S7, R1)
- 세로축: 시간 (ns/us/ms, 항목별 단위 표기)
- 항목별로 PNG를 1장씩 생성한다.

실행:
    python 2026-09-06-String-벤치마크-그래프.py
결과:
    ./string_bench_graphs/*.png (총 13장)

데이터 출처: jc_gbench Release/x64 실행 결과 (CPU Time 기준).
데이터셋 인덱스: 0 Empty, 1 Tiny(7B), 2 Short(22B), 3 Mid(40B),
                 4 Long(128B), 5 Huge(4096B), 6 Utf8Mixed
"""

import os

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

OUT_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "string_bench_graphs")
os.makedirs(OUT_DIR, exist_ok=True)

CANDIDATES = ["R0", "S0", "S1", "S2", "S3", "S4", "S5", "S6", "S7", "R1"]

# 후보별 막대 색 (계열별 구분: 기준=회색, SSO=녹색, Impl=주황, XThread=파랑, 참조=보라)
COLORS = {
    "R0": "#9e9e9e", "S0": "#616161",
    "S1": "#43a047",
    "S2": "#fb8c00", "S3": "#f9a825", "S4": "#ef6c00",
    "S5": "#1e88e5", "S6": "#3949ab", "S7": "#00838f",
    "R1": "#8e24aa",
}
BAR_COLORS = [COLORS[c] for c in CANDIDATES]


def bar_single(title, filename, values, unit, log=False):
    """단일 값 항목 (후보 10개 막대 1장)."""
    fig, ax = plt.subplots(figsize=(11, 5.5))
    bars = ax.bar(CANDIDATES, values, color=BAR_COLORS, edgecolor="black", linewidth=0.6)
    ax.set_title(title, fontsize=13, fontweight="bold")
    ax.set_xlabel("Candidate")
    ax.set_ylabel("Time (%s, lower is better)" % unit)
    if log:
        ax.set_yscale("log")
    ax.grid(axis="y", linestyle="--", alpha=0.5)
    for b, v in zip(bars, values):
        ax.text(b.get_x() + b.get_width() / 2, b.get_height(), "%.3g" % v,
                ha="center", va="bottom", fontsize=8)
    fig.tight_layout()
    fig.savefig(os.path.join(OUT_DIR, filename), dpi=150)
    plt.close(fig)
    print("saved", filename)


def bar_grouped(title, filename, series, series_labels, unit):
    """그룹 막대 (데이터셋/스레드 수 등 시리즈별). series = list of list."""
    n_cand = len(CANDIDATES)
    n_ser = len(series)
    width = 0.8 / n_ser
    x = list(range(n_cand))
    fig, ax = plt.subplots(figsize=(13, 6))
    for i, (vals, label) in enumerate(zip(series, series_labels)):
        off = [(xi - 0.4 + width / 2 + i * width) for xi in x]
        ax.bar(off, vals, width=width, label=label, edgecolor="black", linewidth=0.4)
    ax.set_title(title, fontsize=13, fontweight="bold")
    ax.set_xlabel("Candidate")
    ax.set_ylabel("Time (%s, lower is better, log scale)" % unit)
    ax.set_xticks(x)
    ax.set_xticklabels(CANDIDATES)
    ax.set_yscale("log")
    ax.legend(fontsize=8, ncol=4)
    ax.grid(axis="y", linestyle="--", alpha=0.5)
    fig.tight_layout()
    fig.savefig(os.path.join(OUT_DIR, filename), dpi=150)
    plt.close(fig)
    print("saved", filename)


# ---------------------------------------------------------------- 데이터 (CPU Time)
# Append1B (us)
bar_single("BM_String_Append1B (1B x N append, us, lower is better)",
           "01_append1b.png",
           [2.51, 2.46, 2.46, 3.53, 3.45, 2.57, 3.21, 3.30, 2.76, 2.98], "us")

# ConcatChain (ns) - 편차가 커서 log
bar_single("BM_String_ConcatChain (operator+ chain, ns, lower is better, log)",
           "02_concatchain.png",
           [230, 230, 39.0, 1074, 255, 50.0, 67.0, 82.0, 77.4, 29.5], "ns", log=True)

# Format (ns)
bar_single("BM_String_Format (ns, lower is better)",
           "03_format.png",
           [663, 645, 586, 1283, 645, 586, 586, 600, 586, 645], "ns")

# FindCompare (ns)
bar_single("BM_String_FindCompare (ns, lower is better)",
           "04_findcompare.png",
           [8.54, 8.16, 9.42, 12.0, 12.6, 12.2, 12.3, 11.4, 12.0, 16.5], "ns")

# VectorSort /10000 (ms)
bar_single("BM_String_VectorSort /10000 (ms, lower is better)",
           "05_vectorsort.png",
           [0.893, 0.837, 0.750, 0.889, 0.795, 0.500, 0.586, 0.594, 0.558, 0.367], "ms")

# BinarySearch /10000 (us)
bar_single("BM_String_BinarySearch /10000 (us, lower is better)",
           "06_binarysearch.png",
           [0.330, 0.321, 0.267, 0.392, 0.353, 0.279, 0.279, 0.289, 0.283, 0.251], "us")

DATASET_LABELS = ["0:Empty", "1:Tiny7B", "2:Short22B", "3:Mid40B",
                  "4:Long128B", "5:Huge4096B", "6:Utf8Mixed"]

# Construct (ns) : 후보별 7개 데이터셋
_construct = {
    "R0": [51.6, 58.6, 68.0, 83.7, 172, 3376, 94.2],
    "S0": [50.2, 58.6, 68.0, 83.7, 172, 3442, 94.2],
    "S1": [6.63, 7.15, 13.5, 71.1, 106, 1350, 73.9],
    "S2": [105, 107, 117, 132, 215, 3530, 144],
    "S3": [57.2, 62.8, 73.9, 87.2, 176, 3296, 100],
    "S4": [6.28, 9.63, 13.7, 20.4, 107, 1381, 71.5],
    "S5": [11.0, 13.5, 16.9, 24.6, 110, 1350, 78.5],
    "S6": [14.0, 16.9, 19.9, 27.3, 117, 1413, 81.6],
    "S7": [14.8, 16.0, 19.9, 26.1, 115, 1413, 82.0],
    "R1": [9.24, 11.2, 65.6, 69.8, 100, 1172, 71.5],
}
bar_grouped("BM_String_Construct by dataset (ns, log)",
            "07_construct.png",
            [[_construct[c][d] for c in CANDIDATES] for d in range(7)],
            DATASET_LABELS, "ns")

# CopyConstruct (ns)
_copyctor = {
    "R0": [56.2, 61.4, 71.5, 83.7, 169, 3376, 97.7],
    "S0": [56.2, 61.4, 71.1, 85.8, 176, 3453, 98.4],
    "S1": [7.11, 4.14, 6.98, 57.2, 55.8, 120, 55.8],
    "S2": [103, 107, 123, 138, 220, 3376, 143],
    "S3": [59.4, 64.2, 75.0, 90.0, 176, 3453, 96.3],
    "S4": [6.98, 7.15, 6.98, 9.42, 60.0, 123, 58.6],
    "S5": [12.0, 11.7, 12.2, 13.2, 65.6, 128, 65.6],
    "S6": [15.0, 14.8, 14.4, 16.1, 65.6, 128, 65.6],
    "S7": [14.3, 14.4, 13.8, 16.4, 68.4, 128, 65.6],
    "R1": [3.53, 3.61, 57.2, 57.2, 56.2, 117, 54.4],
}
bar_grouped("BM_String_CopyConstruct by dataset (ns, log)",
            "08_copyconstruct.png",
            [[_copyctor[c][d] for c in CANDIDATES] for d in range(7)],
            DATASET_LABELS, "ns")

# CopyAssign (ns)
_copyassign = {
    "R0": [2.18, 5.58, 13.4, 22.5, 72.5, 2100, 30.5],
    "S0": [2.09, 5.62, 13.4, 22.5, 71.1, 2131, 29.2],
    "S1": [7.11, 4.14, 6.63, 5.86, 5.86, 53.0, 5.86],
    "S2": [2.89, 5.00, 9.63, 15.7, 50.0, 1350, 20.4],
    "S3": [5.30, 8.58, 12.0, 17.6, 51.6, 1350, 22.0],
    "S4": [6.98, 6.98, 6.98, 8.89, 11.7, 57.8, 11.7],
    "S5": [7.39, 7.25, 6.98, 8.79, 10.3, 71.9, 10.3],
    "S6": [6.98, 6.84, 6.56, 9.42, 10.5, 59.4, 10.3],
    "S7": [6.56, 6.56, 6.14, 8.54, 10.5, 39.7, 10.3],
    "R1": [6.70, 7.53, 3.21, 5.58, 5.58, 39.3, 5.62],
}
bar_grouped("BM_String_CopyAssign by dataset (ns, log)",
            "09_copyassign.png",
            [[_copyassign[c][d] for c in CANDIDATES] for d in range(7)],
            DATASET_LABELS, "ns")

# Move (ns)
_move = {
    "R0": [53.0, 58.6, 69.8, 98.4, 180, 3530, 109],
    "S0": [51.6, 58.6, 69.8, 96.3, 180, 3449, 94.2],
    "S1": [9.00, 10.0, 18.8, 68.0, 103, 1381, 73.9],
    "S2": [105, 110, 128, 138, 220, 3369, 146],
    "S3": [57.2, 62.8, 73.2, 87.9, 176, 3296, 95.2],
    "S4": [6.00, 10.0, 15.0, 21.5, 105, 1350, 75.0],
    "S5": [9.52, 13.7, 16.9, 24.0, 110, 1395, 73.2],
    "S6": [13.4, 15.7, 19.5, 26.2, 115, 1413, 80.2],
    "S7": [11.4, 14.1, 18.4, 25.1, 115, 1413, 78.5],
    "R1": [11.0, 13.5, 71.5, 73.2, 103, 1172, 76.7],
}
bar_grouped("BM_String_Move by dataset (ns, log)",
            "10_move.png",
            [[_move[c][d] for c in CANDIDATES] for d in range(7)],
            DATASET_LABELS, "ns")

# MTIndependent (real_time ns) : threads 1 / 4 / 8
_mt = {
    "R0": [167, 51.9, 48.9],
    "S0": [169, 50.9, 49.1],
    "S1": [127, 37.6, 37.1],
    "S2": [267, 80.6, 78.4],
    "S3": [173, 52.0, 50.2],
    "S4": [43.8, 12.2, 7.88],
    "S5": [33.7, 14.4, 8.63],
    "S6": [42.0, 12.8, 8.77],
    "S7": [40.2, 12.7, 8.81],
    "R1": [135, 43.2, 38.9],
}
bar_grouped("BM_String_MTIndependent real_time by threads (ns, log)",
            "11_mtindependent.png",
            [[_mt[c][t] for c in CANDIDATES] for t in range(3)],
            ["threads:1", "threads:4", "threads:8"], "ns")

# CrossThread /100 (ns)
bar_single("BM_String_CrossThread /100 (ns, lower is better)",
           "12_crossthread.png",
           [20996, 20508, 20707, 28565, 25670, 16009, 20926, 16392, 11928, 22949], "ns")

# OwnerExit /100 (ns, log)
bar_single("BM_String_OwnerExit /100 (ns, lower is better, log)",
           "13_ownerexit.png",
           [6557, 6411, 6406, 11091, 5625, 1918, 3830, 15067, 10156, 5313], "ns", log=True)

print("done ->", OUT_DIR)
