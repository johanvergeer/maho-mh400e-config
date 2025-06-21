from gitlint.rules import CommitRule, RuleViolation
import spacy

# Laad spaCy model (cache op module-niveau om performance te sparen)
nlp = spacy.load("en_core_web_sm")

class ImperativeMoodTitle(CommitRule):
    name = "imperative-mood-title"
    id = "UC2"

    def validate(self, commit):
        title = commit.message.title.strip()
        if not title:
            return []

        doc = nlp(title)
        for token in doc:
            if not token.is_space:
                if token.pos_ != "VERB" or token.tag_ != "VB":
                    return [
                        RuleViolation(
                            self.id,
                            f"Title must start with a verb in imperative mood (found '{token.text}')",
                            title
                        )
                    ]
                break

        return []
