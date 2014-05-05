# AdjnL: P VERB<soñar>
(@temp) = ($listTags =~ /($P$a)($VERB${l}soñar\|${r})/g);
$Rel =  "AdjnL";
DepHead($Rel,"",@temp);
$listTags =~ s/($P$a)($VERB${l}soñar\|${r})/$2/g;

# AdjnL: ADV<lemma:$Quant> ADJ|ADV
# Recursivity: 1
# Agreement:
# Inherit:
# Add:
(@temp) = ($listTags =~ /($ADV${l}lemma:$Quant\|${r})($ADJ$a|$ADV$a)/g);
$Rel =  "AdjnL";
DepHead($Rel,"",@temp);
$listTags =~ s/($ADV${l}lemma:$Quant\|${r})($ADJ$a|$ADV$a)/$2/g;
(@temp) = ($listTags =~ /($ADV${l}lemma:$Quant\|${r})($ADJ$a|$ADV$a)/g);
$Rel =  "AdjnL";
DepHead($Rel,"",@temp);
$listTags =~ s/($ADV${l}lemma:$Quant\|${r})($ADJ$a|$ADV$a)/$2/g;

# SpecL: P<token:[Ss]e> VERB
(@temp) = ($listTags =~ /($P${l}token:[Ss]e\|${r})($VERB$a)/g);
$Rel =  "SpecL";
DepHead($Rel,"",@temp);
$listTags =~ s/($P${l}token:[Ss]e\|${r})($VERB$a)/$2/g;

# SpecR: VERB P<token:[Ss]e>
(@temp) = ($listTags =~ /($VERB$a)($P${l}token:[Ss]e\|${r})/g);
$Rel =  "SpecR";
HeadDep($Rel,"",@temp);
$listTags =~ s/($VERB$a)($P${l}token:[Ss]e\|${r})/$1/g;

# AdjnL: [VERB] ADV VERB
(@temp) = ($listTags =~ /(?:$VERB$a)($ADV$a)($VERB$a)/g);
$Rel =  "AdjnL";
DepHead($Rel,"",@temp);
$listTags =~ s/($VERB$a)($ADV$a)($VERB$a)/$1$3/g;

# AdjnL: ADV VERB
# Recursivity: 1
(@temp) = ($listTags =~ /($ADV$a)($VERB$a)/g);
$Rel =  "AdjnL";
DepHead($Rel,"",@temp);
$listTags =~ s/($ADV$a)($VERB$a)/$2/g;
(@temp) = ($listTags =~ /($ADV$a)($VERB$a)/g);
$Rel =  "AdjnL";
DepHead($Rel,"",@temp);
$listTags =~ s/($ADV$a)($VERB$a)/$2/g;

# AdjnR: VERB ADV
# Recursivity: 1
(@temp) = ($listTags =~ /($VERB$a)($ADV$a)/g);
$Rel =  "AdjnR";
HeadDep($Rel,"",@temp);
$listTags =~ s/($VERB$a)($ADV$a)/$1/g;
(@temp) = ($listTags =~ /($VERB$a)($ADV$a)/g);
$Rel =  "AdjnR";
HeadDep($Rel,"",@temp);
$listTags =~ s/($VERB$a)($ADV$a)/$1/g;

# SpecL: VERB<type:S> VERB<mode:P>
# Inherit: tense, person, number
# Add: voice:passive
(@temp) = ($listTags =~ /($VERB${l}type:S\|${r})($VERB${l}mode:P\|${r})/g);
$Rel =  "SpecL";
DepHead($Rel,"",@temp);
$listTags =~ s/($VERB${l}type:S\|${r})($VERB${l}mode:P\|${r})/$2/g;
Inherit("DepHead","tense,person,number",@temp);
Add("DepHead","voice:passive",@temp);

# SpecL: VERB<type:A> VERB<mode:P>
# Inherit: tense, mode, person, number
# Add: type:perfect
(@temp) = ($listTags =~ /($VERB${l}type:A\|${r})($VERB${l}mode:P\|${r})/g);
$Rel =  "SpecL";
DepHead($Rel,"",@temp);
$listTags =~ s/($VERB${l}type:A\|${r})($VERB${l}mode:P\|${r})/$2/g;
Inherit("DepHead","tense,mode,person,number",@temp);
Add("DepHead","type:perfect",@temp);

# SpecL: VERB VERB<mode:N>
# Recursivity: 1
# Inherit: mode, person, tense, number
(@temp) = ($listTags =~ /($VERB$a)($VERB${l}mode:N\|${r})/g);
$Rel =  "SpecL";
DepHead($Rel,"",@temp);
$listTags =~ s/($VERB$a)($VERB${l}mode:N\|${r})/$2/g;
Inherit("DepHead","mode,person,tense,number",@temp);
(@temp) = ($listTags =~ /($VERB$a)($VERB${l}mode:N\|${r})/g);
$Rel =  "SpecL";
DepHead($Rel,"",@temp);
$listTags =~ s/($VERB$a)($VERB${l}mode:N\|${r})/$2/g;
Inherit("DepHead","mode,person,tense,number",@temp);

# CprepL: VERB PRP<lemma:$PrepLocs>|CONJ<lemma:que&type:S> VERB<mode:N>
# Recursivity: 1
# Inherit: mode, person, tense, number
(@temp) = ($listTags =~ /($VERB$a)($PRP${l}lemma:$PrepLocs\|${r} |$CONJ${l}lemma:que\|${b}type:S\|${r})($VERB${l}mode:N\|${r})/g);
$Rel =  "CprepL";
DepRelHead($Rel,"",@temp);
$listTags =~ s/($VERB$a)($PRP${l}lemma:$PrepLocs\|${r} |$CONJ${l}lemma:que\|${b}type:S\|${r})($VERB${l}mode:N\|${r})/$3/g;
Inherit("DepRelHead","mode,person,tense,number",@temp);
(@temp) = ($listTags =~ /($VERB$a)($PRP${l}lemma:$PrepLocs\|${r} |$CONJ${l}lemma:que\|${b}type:S\|${r})($VERB${l}mode:N\|${r})/g);
$Rel =  "CprepL";
DepRelHead($Rel,"",@temp);
$listTags =~ s/($VERB$a)($PRP${l}lemma:$PrepLocs\|${r} |$CONJ${l}lemma:que\|${b}type:S\|${r})($VERB${l}mode:N\|${r})/$3/g;
Inherit("DepRelHead","mode,person,tense,number",@temp);

# AdjnL: ADJ|CARD NOUN
# Recursivity: 1
# Agr: number, genre
(@temp) = ($listTags =~ /($ADJ$a|$CARD$a)($NOUN$a)/g);
$Rel =  "AdjnL";
DepHead($Rel,"number,genre",@temp);
$listTags =~ s/($ADJ${l}concord:1${r}|$CARD${l}concord:1${r})($NOUN${l}concord:1${r})/$2/g;
$listTags =~ s/concord:[01]\|//g;
(@temp) = ($listTags =~ /($ADJ$a|$CARD$a)($NOUN$a)/g);
$Rel =  "AdjnL";
DepHead($Rel,"number,genre",@temp);
$listTags =~ s/($ADJ${l}concord:1${r}|$CARD${l}concord:1${r})($NOUN${l}concord:1${r})/$2/g;
$listTags =~ s/concord:[01]\|//g;

# AdjnR: NOUN ADJ|CARD
# Recursivity: 1
# Agr: number, genre
(@temp) = ($listTags =~ /($NOUN$a)($ADJ$a|$CARD$a)/g);
$Rel =  "AdjnR";
HeadDep($Rel,"number,genre",@temp);
$listTags =~ s/($NOUN${l}concord:1${r})($ADJ${l}concord:1${r}|$CARD${l}concord:1${r})/$1/g;
$listTags =~ s/concord:[01]\|//g;
(@temp) = ($listTags =~ /($NOUN$a)($ADJ$a|$CARD$a)/g);
$Rel =  "AdjnR";
HeadDep($Rel,"number,genre",@temp);
$listTags =~ s/($NOUN${l}concord:1${r})($ADJ${l}concord:1${r}|$CARD${l}concord:1${r})/$1/g;
$listTags =~ s/concord:[01]\|//g;

# AdjnR: NOUN NOUN
# Recursivity: 1
(@temp) = ($listTags =~ /($NOUN$a)($NOUN$a)/g);
$Rel =  "AdjnR";
HeadDep($Rel,"",@temp);
$listTags =~ s/($NOUN$a)($NOUN$a)/$1/g;
(@temp) = ($listTags =~ /($NOUN$a)($NOUN$a)/g);
$Rel =  "AdjnR";
HeadDep($Rel,"",@temp);
$listTags =~ s/($NOUN$a)($NOUN$a)/$1/g;

# SpecL: DT NOUN
# Recursivity: 1
# Agr: number, genre
(@temp) = ($listTags =~ /($DT$a)($NOUN$a)/g);
$Rel =  "SpecL";
DepHead($Rel,"number,genre",@temp);
$listTags =~ s/($DT${l}concord:1${r})($NOUN${l}concord:1${r})/$2/g;
$listTags =~ s/concord:[01]\|//g;
(@temp) = ($listTags =~ /($DT$a)($NOUN$a)/g);
$Rel =  "SpecL";
DepHead($Rel,"number,genre",@temp);
$listTags =~ s/($DT${l}concord:1${r})($NOUN${l}concord:1${r})/$2/g;
$listTags =~ s/concord:[01]\|//g;

# CprepR: [NOUN] [PRP<lemma:$Partitive>] NOUN PRP<lemma:$Partitive> NOUN
(@temp) = ($listTags =~ /(?:$NOUN$a)(?:$PRP${l}lemma:$Partitive\|${r})($NOUN$a)($PRP${l}lemma:$Partitive\|${r})($NOUN$a)/g);
$Rel =  "CprepR";
HeadRelDep($Rel,"",@temp);
$listTags =~ s/($NOUN$a)($PRP${l}lemma:$Partitive\|${r})($NOUN$a)($PRP${l}lemma:$Partitive\|${r})($NOUN$a)/$1$2$3/g;

# CprepR: NOUN|P PRP NOUN|P
# Recursivity: 2
(@temp) = ($listTags =~ /($NOUN$a|$P$a)($PRP$a)($NOUN$a|$P$a)/g);
$Rel =  "CprepR";
HeadRelDep($Rel,"",@temp);
$listTags =~ s/($NOUN$a|$P$a)($PRP$a)($NOUN$a|$P$a)/$1/g;
(@temp) = ($listTags =~ /($NOUN$a|$P$a)($PRP$a)($NOUN$a|$P$a)/g);
$Rel =  "CprepR";
HeadRelDep($Rel,"",@temp);
$listTags =~ s/($NOUN$a|$P$a)($PRP$a)($NOUN$a|$P$a)/$1/g;
(@temp) = ($listTags =~ /($NOUN$a|$P$a)($PRP$a)($NOUN$a|$P$a)/g);
$Rel =  "CprepR";
HeadRelDep($Rel,"",@temp);
$listTags =~ s/($NOUN$a|$P$a)($PRP$a)($NOUN$a|$P$a)/$1/g;

# SubjL: [NOUN|P] P<type:R> VERB [NOUN|P]?
# NEXT
# DobjR: [NOUN|P] [P<type:R>] VERB NOUN|P
# NEXT
# SubjL<Relative>: NOUN|P [P<type:R>] VERB [NOUN|P]?
# Agr: number, person
(@temp) = ($listTags =~ /(?:$NOUN$a|$P$a)($P${l}type:R\|${r})($VERB$a)(?:$NOUN$a|$P$a)?/g);
$Rel =  "SubjL";
DepHead($Rel,"",@temp);
(@temp) = ($listTags =~ /(?:$NOUN$a|$P$a)(?:$P${l}type:R\|${r})($VERB$a)($NOUN$a|$P$a)/g);
$Rel =  "DobjR";
HeadDep($Rel,"",@temp);
(@temp) = ($listTags =~ /($NOUN$a|$P$a)(?:$P${l}type:R\|${r})($VERB$a)(?:$NOUN$a|$P$a)?/g);
$Rel =  "SubjL<Relative>";
HeadDep($Rel,"number,person",@temp);
$listTags =~ s/($NOUN${l}concord:1${r}|$P${l}concord:1${r})($P${l}type:R\|${r})($VERB${l}concord:1${r})($NOUN$a|$P$a)?/$1/g;
$listTags =~ s/concord:[01]\|//g;

# DobjL: [NOUN|P] P<type:R> [NOUN] VERB
# NEXT
# SubjL: [NOUN|P] [P<type:R>] NOUN VERB
# Agr: number, person
# NEXT
# DobjL<Relative>: NOUN|P [P<type:R>] [NOUN] VERB
(@temp) = ($listTags =~ /(?:$NOUN$a|$P$a)($P${l}type:R\|${r})(?:$NOUN$a)($VERB$a)/g);
$Rel =  "DobjL";
DepHead($Rel,"",@temp);
(@temp) = ($listTags =~ /(?:$NOUN$a|$P$a)(?:$P${l}type:R\|${r})($NOUN$a)($VERB$a)/g);
$Rel =  "SubjL";
DepHead($Rel,"number,person",@temp);
(@temp) = ($listTags =~ /($NOUN$a|$P$a)(?:$P${l}type:R\|${r})(?:$NOUN$a)($VERB$a)/g);
$Rel =  "DobjL<Relative>";
HeadDep($Rel,"",@temp);
$listTags =~ s/($NOUN$a|$P$a)($P${l}type:R\|${r})($NOUN${l}concord:1${r})($VERB${l}concord:1${r})/$1/g;

# DobjL: NOUN|P VERB<mode:P>
(@temp) = ($listTags =~ /($NOUN$a|$P$a)($VERB${l}mode:P\|${r})/g);
$Rel =  "DobjL";
DepHead($Rel,"",@temp);
$listTags =~ s/($NOUN$a|$P$a)($VERB${l}mode:P\|${r})/$2/g;

# AdjnR<Attribute>: NOUN|P [VERB] ADJ [PRP]? [NOUN|P]?
# Agr:genre,number
# NEXT
# CprepR: [NOUN|P] [VERB] ADJ PRP NOUN|P
# NEXT
# AdjnR: [NOUN|P] VERB ADJ [PRP]? [NOUN|P]?
# NEXT
# SubjL: NOUN|P VERB [ADJ] [PRP]? [NOUN|P]?
# Agr: number, person
(@temp) = ($listTags =~ /($NOUN$a|$P$a)(?:$VERB$a)($ADJ$a)(?:$PRP$a)?(?:$NOUN$a|$P$a)?/g);
$Rel =  "AdjnR<Attribute>";
DepHead($Rel,"genre,number",@temp);
(@temp) = ($listTags =~ /(?:$NOUN$a|$P$a)(?:$VERB$a)($ADJ$a)($PRP$a)($NOUN$a|$P$a)/g);
$Rel =  "CprepR";
HeadRelDep($Rel,"",@temp);
(@temp) = ($listTags =~ /(?:$NOUN$a|$P$a)($VERB$a)($ADJ$a)(?:$PRP$a)?(?:$NOUN$a|$P$a)?/g);
$Rel =  "AdjnR";
HeadDep($Rel,"",@temp);
(@temp) = ($listTags =~ /($NOUN$a|$P$a)($VERB$a)(?:$ADJ$a)(?:$PRP$a)?(?:$NOUN$a|$P$a)?/g);
$Rel =  "SubjL";
DepHead($Rel,"number,person",@temp);
$listTags =~ s/($NOUN${l}concord:1${r}|$P${l}concord:1${r})($VERB${l}concord:1${r})($ADJ${l}concord:1${r})($PRP$a)?($NOUN$a|$P$a)?/$2/g;
$listTags =~ s/concord:[01]\|//g;

# DobjR: VERB NOUN|P|DT
(@temp) = ($listTags =~ /($VERB$a)($NOUN$a|$P$a|$DT$a)/g);
$Rel =  "DobjR";
HeadDep($Rel,"",@temp);
$listTags =~ s/($VERB$a)($NOUN$a|$P$a|$DT$a)/$1/g;

# CprepR: VERB PRP NOUN|P|VERB<mode:N>|DT
(@temp) = ($listTags =~ /($VERB$a)($PRP$a)($NOUN$a|$P$a|$VERB${l}mode:N\|${r}|$DT$a)/g);
$Rel =  "CprepR";
HeadRelDep($Rel,"",@temp);
$listTags =~ s/($VERB$a)($PRP$a)($NOUN$a|$P$a|$VERB${l}mode:N\|${r}|$DT$a)/$1/g;

# SubjL: NOUN|P|DT VERB
# Agr: number, person
(@temp) = ($listTags =~ /($NOUN$a|$P$a|$DT$a)($VERB$a)/g);
$Rel =  "SubjL";
DepHead($Rel,"number,person",@temp);
$listTags =~ s/($NOUN${l}concord:1${r}|$P${l}concord:1${r}|$DT${l}concord:1${r})($VERB${l}concord:1${r})/$2/g;
$listTags =~ s/concord:[01]\|//g;

# SpecL: [VERB] CONJ<type:S> VERB<mode:[^PNG]>
# NEXT
# DobjR: VERB [CONJ<type:S>] VERB<mode:[^PNG]>
(@temp) = ($listTags =~ /(?:$VERB$a)($CONJ${l}type:S\|${r})($VERB${l}mode:[^PNG]\|${r})/g);
$Rel =  "SpecL";
DepHead($Rel,"",@temp);
(@temp) = ($listTags =~ /($VERB$a)(?:$CONJ${l}type:S\|${r})($VERB${l}mode:[^PNG]\|${r})/g);
$Rel =  "DobjR";
HeadDep($Rel,"",@temp);
$listTags =~ s/($VERB$a)($CONJ${l}type:S\|${r})($VERB${l}mode:[^PNG]\|${r})/$1/g;

