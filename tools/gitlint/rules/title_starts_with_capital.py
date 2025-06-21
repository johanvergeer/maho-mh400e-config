from gitlint.rules import CommitRule
from gitlint.rules import RuleViolation

class TitleStartsWithCapital(CommitRule):
    name = "title-starts-with-capital"
    id = "UC1"

    def validate(self, commit):
        title = commit.message.title
        if title and not title[0].isupper():
            return [RuleViolation(self.id, "Title must start with a capital letter", title)]
        return []
