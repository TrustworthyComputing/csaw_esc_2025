ESC 2025 Challenge Description
==============================

This year's challenge will focus on **side channel and fault injection attacks**, supercharged by AI. Teams are encouraged to leverage deep learning and Large Language Models (LLMs) to automate these hardware attacks, and exfiltrate sensitive data. Competitors will demonstrate their skills using the popular **Chipwhisperer** platform in a red team/blue team scenario to either launch or mitigate these powerful attacks. Competitors will demonstrate their skills through a qualification phase and a final phase. In the final phase teams will investigate a range of attacks and solve educational challenges.

## Challenge structure

The ESC25 competition is divided into two phases:

1. A preliminary **qualification phase**, where teams must compile and submit a short written report. The report should discuss side channel and fault injection attacks, how AI/LLM tools can help automate the process, as well as different mitigations that can be put in place to prevent such attacks, and a potential solution to the short qualification challenge. 


2. A **final phase**, where qualified teams are provided an Chipwhisperer platform to investigate SCAs and FIAs. Participants will demonstrate their solutions to the provided technical challenges and discuss potential mitigations.

See below for more details on the requirements of each phase.


### Qualification Phase

For the qualification phase, teams should submit a **short report** that outlines potential approaches, techniques, and mitigations of the [provided short qualification challenge](https://github.com/TrustworthyComputing/csaw_esc_2025/tree/main/challenges/qualification). Successful reports should include a discussion of existing techniques, a clear outline of attack methodologies, and a discussion of how the corresponding attacks could be mitigated.

Qualification phase reports will be evaluated by a team of experts, and will take into account the **correctness** and **creativity** of reported techniques, as well as the completeness and quality of the compiled report.

## Final Phase Evaluation and Grading Policies

The final phase will be graded as follows:
- 30% of the final score will be **correctness**. The points awarded in this section are based on successfully finding, exploiting, and  mitigating the provided challenges and depend on the difficulty of each challenge. The awarded points will be determined systematically by the global organizers and the expert judges. Partial solutions will not receive technical credit but may hold weight with the judges.
- 20% of the final score will be **AI integration**. This portion of the score is awarded by the panel of expert judge for the creative and effective use of AI/ML/LLM tools to assist or automate the discovery, exploitation, reduction of queries, and/or mitigation of vulnerabilities.
- 20% of the score will be **performance** and **efficiency**. Performance will be evaluated by the panel of expert judges and will encompass the techniques that the participants utilize to address the challenges. The metrics include, but are not limited to:
    - Effectiveness of proposed attacks
    - Efficiency (number of attempts/queries), repeatability, and creativity
    - Automation of attacks
- 30% of the score will be the **quality** of the final deliverables (report, judges' presentation, poster). The final deliverables will be graded by the judges panel based on organization, clearness of presentation, and detail of explanations.

**Note:
Solutions that involve reverse engineering of the flag from provided binaries, ROP programming or modification of provided hex files will not be accepted.**


You can refer to the [deliverables page](deliverables.md) for more details on the qualification and final phase deliverables.
<!-- , and the [Final Phase page](Final_Phase.md) for details about how to get started with this year's challenges. -->

To find more information regarding how to register and participate, click [here](logistics.md).


