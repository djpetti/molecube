# Molecube

*Modular, tactile children's toy.*

As digital technology continues to permeate nearly every aspect of our day-to-day lives, there has lately been a significant amount of focus on better leveraging computers in the classroom. Indeed, this is not a new idea; some of us are probably old enough to remember programming on PDP minicomputers in High School. As technology improves, however, the possible applications for it within the field of education are moving far beyond geeky teenagers, and seem poised to radically affect the educational experience of nearly every child in this country.

It's difficult to argue with the general premise of this idea. Indeed, one could
consider primary education to be ripe for innovation; after all, despite a
century of technological and social progress, the basic paradigm of schooling,
as a set of auditory-visual lessons delivered by a teacher to a room full of
students, has changed remarkably little. This is in spite of mounting evidence
that the traditional, lecture-based method of teaching children is not optimally
effective. The book "Ways of Learning" identifies a full seven different dimensions of intelligence, six of which are not well catered-to by traditional schooling.

In recent years, advances into the primary education market have been made by well known companies such as Apple and Microsoft. When it comes down to it, however, most of the products being offered are traditional laptops, or perhaps tablets if the company in question is feeling especially creative. While a valid need has clearly been identified by these companies, their method of addressing it is woefully misguided.

Consider, for instance, the attributes of a standard tablet:

- Large and clunky.
- Fragile.
- Expensive.
- Internet-connected.

It's not exactly an ideal product for children...

Ultimately, the problems associated with the existing products on the market
seem to boil down to a design choice: Companies seem to think that they can take
products designed for adults and adapt them for use by children, in the process
changing the software and very little else. This approach is fundamentally
misguided, and is likely to result in nothing aside from failure, frustration,
and the funneling of money from your public schools to the Cayman Islands.
Ultimately, then, the problem is that education is ripe for a technology-driven
overhaul, but the major companies do not seem to have taken the time to design a
solution *properly*.

## How it works

Molecube is a novel device which can't be easily slotted into the categories
that we're used to. It's designed to replace much of the existing technology
that most children interact with on a daily basis, with an emphasis on
facilitating education in a hands-on manner. As its name suggests, it features a
novel user interface designed such that a user interacts with it primarily through physical manipulation.

In order to allow for interaction, the device provides visual, audio, and
tactile readout. This is accomplished with a small display, a speaker, and a vibration motor, respectively. The device's distinguishing characteristic, however, is it's ability to be physically linked with other devices. When linked together, the devices are not only magnetically attached, but can also communicate with one another. This interaction is the backbone of the user interface.

### Software

Each cube is its own Linux system, complete with onboard storage. The cubes are
capable of running third-party applications, which is where most of their
functionality comes from. There will be a standard set of APIs for applications
to use. Linking agains the API will allow them to leverage both the actual
hardware, and the simulator.

Since the cubes have no wireless networking, application administration is
handled through the cube's charging base. This is something that will likely be
done by parents, rather than kids.

## Developing

We welcome contributors. Below are instructions on building and running the
code.

## Simulator

The simulator can be run as follows:

```
~$ bazel run //simulator:demo_game
```

Make sure that you use Python 2 for this and that you have Tkinter installed.

## Applications and LibMC

To build the applications and library, use [bazel](https://bazel.build/):

```
bazel build -c dbg ...
```

All artifacts will be located in the `bazel-out` directory.
