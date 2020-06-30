# cagliostro
**cagliostro** is a tool for creating cross-platform multimedia surveys for empirical experiments without coding. Using FFMPEG as backend, it provides platform-independent videos and utilizes standard XML and tab-separated values as input and output, respectively. Exemplary, you find an example input file below:

```xml
<!-- The root element - specifies the title of the survery, the participant and where to save the result-->
<cagliostro title="A Quokka survey" participant="abc" result="result.tsv">
    <!-- Some pages supporting Markdown -->
    <page id="start" next_button="I swear!">
        <title>General information for participant</title>
        <description>I solemnly swear that I'm a **Quokka** lover (and have seen the *amazing* Markdown).</description>
    </page>

    <!-- The "randomized_pages" element... well.. randomize the order of its pages -->
    <randomized_pages>
        <page id="video1">
            <title>Please rate the cuteness of this Quokka:</title>
            <content type="video" obligatory="true">quokka1.mp4</content>
            <question id="cuteness1">
                <description>Cuteness</description>
                <choices low="Extrem" high="Not so extreme">
                    <choice>Extreme cute</choice>
                    <choice>Super cute</choice>
                    <choice>Cute</choice>
                </choices>
            </question>
        </page>

        <page id="video2">
            <title>Please rate the cuteness of this Quokka (2):</title>
            <!-- Content may be specified relative to the input file, but i.e. even as HTTP URL -->
            <content type="video">quokka2.mp4</content>
            <question id="cuteness2">
                <description>Cuteness</description>
                <choices>
                    <choice>Extreme cute</choice>
                    <choice>Super cute</choice>
                    <choice>Cute</choice>
                </choices>
            </question>
        </page>
    </randomized_pages>

    <page id="end">
        <title>The end</title>
        <description>Thank you for your time!</description>
    </page>
</cagliostro>
```

Such a file may be understood as a "master file" or a template. For each subject, the experimentators may
1. Modify the "participant" attribute of the element "cagliostro".
2. Optionally, modify the header of the video files on a individual basis. Using the provided utility script, such a command may look like `python scripts/replace_header.py stimulus.mp4 HEX_REPLACEMENT`. The output of the script may than be set as the "header" attribute on the corresponding "content" element. As an alternative, this step may be utilized once for all participants, too.
3. Optionally, randomize the order of pages. Using the provided utility script, such a command may look like `python scripts/shuffle_pages.py TEMPLATE_FILE.cagliostro SUBJECT_FILE.cagliostro`
4. Optionally, encrypt the file. Using the compiled crypto utility, such a command may look like `cagliostro_crypto[.exe] SUBJECT_FILE.cagliostro ENCRYPTED_SUBJECT_FILE.cagliostro PASSWORD`

This file may than be distributed to the subject. If both the stimuli, the config file and the the executable are shared all together, the participant does not have to select such a file manually.
