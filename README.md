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

    <!-- Pages without content might be used for checks -->
    <page id="checks">
        <description>Please provide some details about yourself:</description>
        <question id="gender">
            <description>Your gender</description>
            <choices>
                <choice>Male</choice>
                <choice>Female</choice>
            </choices>
        </question>
    </page>

    <!-- The "randomized_pages" element... well.. randomize the order of its pages -->
    <randomized_pages>
        <page id="video1">
            <description>Please rate the cuteness of this Quokka:</description>
            <!-- Content may be specified relative to the input file, but i.e. even as HTTP URL -->
            <content type="video">quokka1.mp4</content>
            <question id="cuteness1">
                <description>Cuteness</description>
                <choices>
                    <choice>Extreme cute</choice>
                    <choice>Super cute</choice>
                    <choice>Cute</choice>
                </choices>
            </question>
        </page>
        <page id="video2">
            <description>Please rate the cuteness of this Quokka (2):</description>
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